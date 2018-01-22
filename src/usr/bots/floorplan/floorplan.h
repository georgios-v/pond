

#ifndef FLOORPLAN_H
#define FLOORPLAN_H


void floorplan_init(char *);
void floorplan_end (void);
void compute_floorplan(opt_worker_t self);
int floorplan_verify(void);
int floorplan(APP_ARGS);

#endif