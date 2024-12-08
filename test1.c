#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 100

typedef struct {
    pthread_mutex_t mutex;
    pthread_cond_t cond_sender;
    pthread_cond_t cond_receiver;
    char message[MAX_MESSAGE_LENGTH];
    int turn; // 0 - Thread 1 trimite, 1 - Thread 2 trimite
} shared_data;

void *thread_1(void *arg) {
    shared_data *data = (shared_data *)arg;

    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&data->mutex);

        // Așteaptă să fie rândul său să trimită
        while (data->turn != 0) {
            pthread_cond_wait(&data->cond_sender, &data->mutex);
        }

        // Trimite mesajul
        snprintf(data->message, MAX_MESSAGE_LENGTH, "Hello from thread 1 (%d)", i + 1);
        printf("[Thread 1] Sent: %s\n", data->message);

        // Schimbă rândul
        data->turn = 1;
        pthread_cond_signal(&data->cond_receiver);

        // Așteaptă să primească mesajul de la Thread 2
        while (data->turn != 0) {
            pthread_cond_wait(&data->cond_sender, &data->mutex);
        }

        printf("[Thread 1] Received: %s\n", data->message);

        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}

void *thread_2(void *arg) {
    shared_data *data = (shared_data *)arg;

    for (int i = 0; i < 5; i++) {
        pthread_mutex_lock(&data->mutex);

        // Așteaptă mesajul de la Thread 1
        while (data->turn != 1) {
            pthread_cond_wait(&data->cond_receiver, &data->mutex);
        }

        printf("[Thread 2] Received: %s\n", data->message);

        // Trimite răspunsul
        snprintf(data->message, MAX_MESSAGE_LENGTH, "Hello from thread 2 (%d)", i + 1);
        printf("[Thread 2] Sent: %s\n", data->message);

        // Schimbă rândul
        data->turn = 0;
        pthread_cond_signal(&data->cond_sender);

        pthread_mutex_unlock(&data->mutex);
    }

    return NULL;
}

int main() {
    pthread_t t1, t2;
    shared_data data;

    // Inițializare date partajate
    pthread_mutex_init(&data.mutex, NULL);
    pthread_cond_init(&data.cond_sender, NULL);
    pthread_cond_init(&data.cond_receiver, NULL);
    data.turn = 0; // Thread-ul 1 începe

    // Creare thread-uri
    pthread_create(&t1, NULL, thread_1, &data);
    pthread_create(&t2, NULL, thread_2, &data);

    // Așteaptă finalizarea thread-urilor
    pthread_join(t1, NULL);
    pthread_join(t2, NULL);

    // Curățare resurse
    pthread_mutex_destroy(&data.mutex);
    pthread_cond_destroy(&data.cond_sender);
    pthread_cond_destroy(&data.cond_receiver);

    return 0;
}
