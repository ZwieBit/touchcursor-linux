#ifndef config_h
#define config_h

/**
 * The event path for the device.
 * */
extern char eventPath[4][18];

/**
 * The hyper key;
 * */
extern int hyperKey;

/**
 * Map for keys and their conversion.
 * */
extern int keymap[256];

/**
 * Map for permanently remapped keys.
 * */
extern int remap[256];

/**
 * Number of devices which are configurd
 * */
extern int devices;

/**
 * Reads the configuration file.
 * */
void readConfiguration();

#endif
