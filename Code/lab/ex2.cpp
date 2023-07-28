#include <cstdio>
#include <thread>
#include <mutex>
#include <queue>
#include <cstdlib>      // rand
#include <chrono>
#include <condition_variable>

namespace {
    std::queue<int> queue_;
    std::mutex      mutex_;
    std::condition_variable cv_;
    bool should_run_ = true;
}

void add_to_queue(int v)
{
    // Fournit un accès synchronisé à queue_ pour l'ajout de valeurs.
    std::unique_lock<std::mutex> lock(mutex_);
    queue_.push(v);
    cv_.notify_one();
}

void producteur()
{
    // Produit 100 nombres aléatoires de 1000 à 2000 et les additionne
    // à une file d'attente (queue_) pour traitement.
    // À la fin, transmet "0", ce qui indique que le travail est terminé.

    for (int i = 0; i < 100; ++i)
    {
        int r = rand() % 1001 + 1000;
        add_to_queue(r);

        // Bloque le fil pour 50 ms:
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    add_to_queue(0);
}

void consommateur()
{
    while (should_run_)
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, []{ return !queue_.empty(); });
        // On doit toujours vérifier si un objet std::queue n'est pas vide
        // avant de retirer un élément.
        if (!queue_.empty()) {
            int v = queue_.front(); // Copie le premier élément de la queue.
            queue_.pop();           // Retire le premier élément.

            if (v != 0)
                printf("Reçu: %d\n", v);
            else
                // Travail terminé, on quitte la fonction du consommateur.
                return;
        }
    }
}

int main(int argc, char** argv)
{
    auto start_time = std::chrono::high_resolution_clock::now();

    std::thread thread_producteur(producteur);
    std::thread thread_consommateur(consommateur);

    thread_producteur.join();
    should_run_ = false;    // Signal de terminaison pour le consommateur
    thread_consommateur.join();

    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
    printf("Le programme a pris %lld ms.\n", duration.count());

    return 0;
}

