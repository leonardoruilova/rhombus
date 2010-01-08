#ifndef DRIVER_H
#define DRIVER_H

#include <stdint.h>
#include <stdbool.h>

/***** PORT ACCESS *****/
uint8_t inb(uint16_t port);
uint16_t inw(uint16_t port);
uint16_t ind(uint16_t port);

void outb(uint16_t port, uint8_t value);
void outw(uint16_t port, uint16_t value);
void outd(uint16_t port, uint32_t value);

void iodelay(uint32_t usec);

/***** DEVICE DESCRIPTOR *****/

/* 	device_t specifics:
 * 		bits 00-07: device number
 * 		bits 08-15: bus number
 * 		bits 16-23: reserved
 * 		bits 24-30: bus type
 * 		bit 31: validity bit
 */

typedef int32_t device_t;

bool dev_check(device_t dev);
uint16_t dev_getdevice(device_t dev);
uint16_t dev_getvendor(device_t dev);
uint16_t dev_getclass(device_t dev);
uint16_t dev_getiobase(device_t dev, int bar);
uint16_t dev_getiolimit(device_t dev, int bar);
uint16_t dev_getstatus(device_t dev);
uint16_t dev_command(device_t dev, uint16_t command);

/***** REQUEST STRUCTURE *****/

struct request {
	uint8_t checksum	[4]; /* Checksum (byte parity) */
	uint8_t resource	[4]; /* Resource ID */
	uint8_t datasize	[2]; /* Size of request data */
	uint8_t transaction	[2]; /* Transaction ID */
	uint8_t dataoff		[2]; /* Offset of request data */ 
	uint8_t format		[2]; /* Header format */
	uint8_t fileoff		[16]; /* File offset */
};

/* locally accessible decoding of request structure */
struct localrequest {
	uint32_t resource;
	uint16_t transaction;
	uint32_t offset;
	void *data;
	size_t datasize;
};

void req_decode(struct request *r, struct localrequest *l);
void req_encode(struct localrequest *l, struct request *r);
struct request *req_alloc(void);

/***** DRIVER INTERFACE STRUCTURE *****/ 

#define DRV_DONE 0
#define DRV_ERROR -1
#define DRV_WAIT 1

typedef void (*callback_t)(struct request *r);
struct driver_interface {
	int (*init) (device_t dev);	/* Initialize driver on device */
	int (*sleep) (void);		/* Put device into powersave */
	int (*halt) (void);			/* De-initialize device */

	int (*read) (struct request *, callback_t);	/* Read request */
	int (*write)(struct request *, callback_t); /* Write request */
	int (*ctrl) (struct request *, callback_t); /* Control request */
	int (*info) (struct request *, callback_t); /* Information request */

	void (*work) (void);	/* Do background work */
	int jobs;				/* Number of background jobs to complete */

	void (*handler) (void);	/* To be called on IRQs */
	int16_t irq;			/* IRQ to be caught (-1 means none) */
};

#endif