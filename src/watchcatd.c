#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <signal.h>

#include <libnotify/notify.h>

#define BUFFER_MAX          4096

#define ERR_ARGS            1
#define ERR_MEM_ALLOC       2
#define ERR_INIT_INOTIFY    3
#define ERR_INOTIFY_ADD     4
#define ERR_INOTIFY_READ    5
#define ERR_DAEMON          6
#define ERR_NOTIFY_INIT     7

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

static void handle_file_changes(char *file_name, char *file_path) {
  char *notification_msg = NULL;
  char buffer[BUFFER_MAX];
  int read_length;
  int ret;
  
  char *program_title = "watchcatd";

  NotifyNotification *notify_handle;

  ret = notify_init(program_title);
  if (!ret) {
    perror("Error initilizing notify: ");
    exit(ERR_NOTIFY_INIT);
  }

  const struct inotify_event *watch_event;
  const int watch_mask = IN_DELETE | IN_ACCESS | IN_MODIFY; 

  IEventQueue = inotify_init();
  if (IEventQueue == -1) {
    perror("Error initilizing inotify instance: ");
    exit(ERR_INIT_INOTIFY);
  }

  IEventStatus = inotify_add_watch(IEventQueue, file_path, watch_mask);
  if (IEventStatus == -1) {
    perror("Error adding file to watch instance: ");
    exit(ERR_INOTIFY_ADD);
  }
  
  while (1) {
     //fprintf(stdout, "Waiting for event...\n");

     read_length = read(IEventQueue, buffer, BUFFER_MAX);
     if (read_length == -1) {
       perror("Error reading for inotify instance: ");
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

       //fprintf(stdout, "%s\n", notification_msg);
       notify_handle = notify_notification_new(file_name, notification_msg, "dialog-information");

       if (!notify_handle) {
         perror("Handle was null: ");
         continue;
       }

       notify_notification_show(notify_handle, NULL);
     }
  }  
}

void signal_handler() {
  int ret;
  
  printf("Exit signal recieved!\nClosing down daemon...\n");

  ret = inotify_rm_watch(IEventQueue, IEventStatus);
  if (ret == -1) {
    perror("Error removing file from watch!");
  }
  
  close(IEventQueue);
  notify_uninit();
  
  exit(EXIT_SUCCESS);
}

int main(int argc, char **argv) {
  int ret = daemon(1, 1);
  if (ret == -1) {
    perror("Failed to daemonize: ");
    exit(ERR_DAEMON);
  }

  signal(SIGINT, signal_handler);
  
  char *base_path = NULL;
  if (argc != 2) {
    fprintf(stderr, "USAGE: watchcatd <PATH>");
    exit(ERR_ARGS);
  }

  base_path = (char *)malloc(sizeof(char) * strlen(argv[1]) + 1);
  if (!base_path) {
    perror("Memory allocation failed: ");
    exit(ERR_MEM_ALLOC);
  }

  strcpy(base_path, argv[1]);
  get_filename(&base_path);
  //fprintf(stdout, "Filename: %s\n", base_path);

  handle_file_changes(base_path, argv[1]);
  
  return 0;
}
