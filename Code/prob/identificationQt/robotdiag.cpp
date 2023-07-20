#include "robotdiag.hpp"
#include "robotsim.hpp"

#include <vector>
#include <queue>
#include <cstdio>
#include <thread>
#include <mutex>
#include <condition_variable>

using namespace s3gro;

// Variables globales
std::thread t_csv_;
std::mutex mutex_;
std::condition_variable cv_;
int id_moteur_filtre = 0; // Numéro du moteur à filtrer


RobotDiag::RobotDiag()
{
    // Démarre le simulateur:
    // TODO: Supprimer cette ligne si vous testez avec un seul moteur
    robotsim::init(this, 8, 10, 3);   // Spécifie le nombre de moteurs à
                                      // simuler (8) et le délai moyen entre
                                      // les événements (10 ms) plus ou moins
                                      // un nombre aléatoire (3 ms).
}

// Le destructeur sera normalement appellé à la fermeture de l'application.
// Écrit des statistiques à l'écran.
RobotDiag::~RobotDiag() {
    stop_recording();
}

void RobotDiag::push_event(RobotState new_robot_state) {
    std::lock_guard<std::mutex> lock(mutex_);

    // Conserve toutes les données
    data_.push_back(new_robot_state);

    // Ajoute le dernier événement à la file d'exportation
    queue_.push(new_robot_state);
    cv_.notify_one();   // Signale qu'une nouvelle donnée est disponible
}

void RobotDiag::set_csv_filename(const std::string& file_name) {
    csv_filename_ = file_name;
}

void RobotDiag::start_recording() {
    // Indique que le système de diagnostic fonctionne (à mettre à 'false' lors
    // de la fermeture pour interrompre le fil d'exportation).
    run_ = true;

    t_csv_ = std::thread(&RobotDiag::export_loop,this); // Lancement du fil
}

void RobotDiag::stop_recording() {
    // Indique que le système de diagnostic doit être arrêté.
    run_ = false;

    t_csv_.join();  // Fermeture du fil

    robotsim::stop_and_join();

    printf("Final vector size: %zu\n", data_.size());
}

// Fonction d'exportation vers CSV.
// Doit être exécutée dans un fil séparé et écrire dans le fichier CSV
// lorsque de nouvelles données sont disponibles dans queue_.
void RobotDiag::export_loop() {
    if (csv_filename_.empty()) {
        csv_filename_ = "/tmp/robotdiag.csv";
    }

    FILE* out = fopen(csv_filename_.c_str(), "w");

    if (out == NULL) {
        printf("ERROR: Cannot open output file.\n");
        return;
    }

    // En-tête du fichier CSV, respectez le format.
    fprintf(out, "motor_id;t;pos;vel;cmd\n");

    // Synchronisation et écriture.
    while(run_) // l'utilisation d'un booléen permet d'arrêter le thread
    {
        std::unique_lock<std::mutex> lock(mutex_);
        cv_.wait(lock, [this] { return !queue_.empty(); }); // 'this' est nécessaire. Autrement, Qt renvoie une erreur de compilation.
        if (!queue_.empty())
        {
            if (queue_.front().id == id_moteur_filtre) // On peut modifier le numéro du moteur désiré dans Qt
            {
                fprintf(out, "%d;%f;%f;%f;%f;\n",queue_.front().id,queue_.front().t,queue_.front().cur_cmd,queue_.front().cur_pos,queue_.front().cur_vel);

            }
            queue_.pop();
        }
    }
    fclose(out);
}

