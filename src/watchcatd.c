#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>

#define BUFFER_MAX          4096

#define ERR_ARGS            1
#define ERR_MEM_ALLOC       2
#define ERR_INIT_INOTIFY    3
#define ERR_INOTIFY_ADD     4
#define ERR_INOTIFY_READ    5
#define ERR_DAEMON          6

int IEventQueue;
int IEventStatus;

static void get_filename(char **buf ) {
  char *token = NULL;

  token = strtok(*buf, "/");
  while (token != NULL) {
    *buf = token;
    token = strtok(NULL, "/");
  }
}

static void handle_file_changes(char *file_path) {
  char *notification_msg = NULL;
  char buffer[BUFFER_MAX];
  int read_length;

  const struct inotify_event *watch_event;
  const int watch_mask = IN_DELETE | IN_ACCESS | IN_MODIFY; 

  IEventQueue = inotify_init();
  if (IEventQueue == -1) {
    fprintf(stderr, "Error initilizing inotify instance!");
    exit(ERR_INIT_INOTIFY);
  }

  IEventStatus = inotify_add_watch(IEventQueue, file_path, watch_mask);
  if (IEventStatus == -1) {
    fprintf(stderr, "Error adding file to watch instance!");
    exit(ERR_INOTIFY_ADD);
  }
  
  while (1) {
     fprintf(stdout, "Waiting for event...\n");

     read_length = read(IEventQueue, buffer, BUFFER_MAX);
     if (read_length == -1) {
       fprintf(stderr, "Error reading for inotify instance!");
       exit(ERR_INOTIFY_READ);
     }
     
     for (char *buffer_pointer = buffer; buffer_pointer < buffer + read_length; buffer_pointer += sizeof(struct inotify_event) + watch_event->len) {
       notification_msg = NULL;
       watch_event = (const struct inotify_event *) buffer_pointer;

       if (watch_event->mask & IN_DELETE) {
         notification_msg = "File deleted.";
       }

       if (watch_event->mask & IN_ACCESS) {
         notification_msg = "File accessed.";
       }

       if (watch_event->mask & IN_MODIFY) {
         notification_msg = "File modified.";
       }

       if (!notification_msg) {
         continue;
       }

       fprintf(stdout, "%s\n", notification_msg);
     }
  }  

}

int main(int argc, char **argv) {
  int ret = daemon(1, 1);
  if (ret == -1) {
    fprintf(stderr, "Failed to daemonize!");
    exit(ERR_DAEMON);
  }
  
  char *base_path = NULL;
  if (argc != 2) {
    fprintf(stderr, "USAGE: watchcatd <PATH>");
    exit(ERR_ARGS);
  }

  base_path = (char *)malloc(sizeof(char) * strlen(argv[1]) + 1);
  if (!base_path) {
    fprintf(stderr, "Memory allocation failed!");
    exit(ERR_MEM_ALLOC);
  }

  strcpy(base_path, argv[1]);
  get_filename(&base_path);
  //fprintf(stdout, "Filename: %s\n", base_path);

  handle_file_changes(argv[1]);
  
  return 0;
}
