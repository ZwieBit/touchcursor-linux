#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <linux/input.h>
#include <string.h>
#include <pthread.h>

#include "config.h"
#include "binding.h"
#include "emit.h"
#include "mapper.h"

pthread_t thread_tid[4];

void *keyboardThreadFunc(void *vargp) 
{
    int* a = (int*)vargp;
    int val = *a;

    struct input_event inputEvent;
    ssize_t result;
    fprintf(stdout, "info: capturing device %i\n", val);
    while (1) 
    {
        result = read(input[val], &inputEvent, sizeof(inputEvent));
        if (result == (ssize_t) - 1)
        {
            if (errno == EINTR) continue;
        }  
        if (result == (ssize_t)0)
        {
            return NULL;// ENOENT;
        }
        if (result != sizeof(inputEvent))
        { 
            return NULL;// EIO; 
        }
        // We only want to manipulate key presses
        if (inputEvent.type == EV_KEY
            && (inputEvent.value == 0 || inputEvent.value == 1 || inputEvent.value == 2))
        {
            processKey(inputEvent.type, inputEvent.code, inputEvent.value);
        }
        else
        {
            emit(inputEvent.type, inputEvent.code, inputEvent.value);
        }
    }
}

/**
* Main method.
* */
int main(int argc, char* argv[])
{ 
    readConfiguration();
    for (int iDevice=0;iDevice<devices;iDevice++)  
    {
        char localEventPath[18];
        localEventPath[0] = '\0';
        fprintf(stderr, "info: configure device ");
        strcat(localEventPath, eventPath[iDevice*18]); 
        fprintf(stderr, localEventPath);
        fprintf(stderr, "\n");
  
        if (localEventPath[0] == '\0')
        {
            fprintf(stderr, "error: please specify the keyboard device name in the configuration file\n");
            return EXIT_FAILURE;
        } 
        // Bind the input device
        if (bindInput(localEventPath, iDevice) != EXIT_SUCCESS)
        { 
            fprintf(stderr, "error: could not capture the keyboard device\n");
            return EXIT_FAILURE;
        }

        int *arg = malloc(sizeof(*arg));
        *arg = iDevice;
        pthread_create(&thread_tid[iDevice], NULL, keyboardThreadFunc, (void*)arg);
    }
    // Bind the output device
    if (bindOutput() != EXIT_SUCCESS)
    { 
        fprintf(stderr, "error: could not create the virtual keyboard  device\n");
        return EXIT_FAILURE; 
    }
    fprintf(stdout, "info: running\n");
    // Read events
    //pthread_create(&thread_ids[0], NULL, keyboardThreadFunc, NULL);
    pthread_join(thread_tid[0], NULL);

    return EXIT_SUCCESS;
}
