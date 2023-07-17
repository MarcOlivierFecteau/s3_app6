#include <cstdio>
#include <thread>
#include <mutex>
#include <queue>
#include <cstdlib>      // rand

namespace {
    std::queue<int> queue_;
    std::mutex      mutex_;
    bool is_prod_done_ = false;
    bool should_run_ = true;
}

void add_to_queue(int v)
{
    // Fournit un accès synchronisé à queue_ pour l'ajout de valeurs.
    
    std::lock_guard<std::mutex> lock(mutex_);
    queue_.push(v);
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

        // Signale au consommateur qu'une donnée est disponible.
        is_prod_done_ = true;

        // Bloque le fil pour 50 ms:
        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    add_to_queue(0);
}

void consommateur()
{
    while (should_run_)
    {
        std::lock_guard<std::mutex> lock(mutex_);
        // On doit toujours vérifier si un objet std::queue n'est pas vide
        // avant de retirer un élément.
        if (!queue_.empty() && is_prod_done_) {
            int v = queue_.front(); // Copie le premier élément de la queue.
            queue_.pop();           // Retire le premier élément.
            if (queue_.empty()) {
                // Si la queue est vide, on réinitialise le flag de production.
                is_prod_done_ = false;
            }

            printf("Reçu: %d\n", v);
            if (v == 0) {
                // Travail terminé, on quitte la fonction du consommateur.
                return;
            }
        }
    }

}

int main(int argc, char** argv)
{
    std::thread thread_producteur(producteur);
    std::thread thread_consommateur(consommateur);

    thread_producteur.join();
    should_run_ = false;    // Signal de terminaison pour le consommateur
    thread_consommateur.join();

    return 0;
}

