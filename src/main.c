#include <libnotify/notify.h>
#include <sys/inotify.h>

#include <stdio.h> 
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <time.h>

#define RET_SUCCESS           0
#define RET_MISSING_SHELL     1
#define RET_INOTIFY_INIT_FAIL 2
#define RET_INOTIFY_ADD_FAIL  3
#define RET_INOTIFY_READ_FAIL 4

// GOAL: write a daemon in c that when called, prints a random quote of wisdom to stdout
/* STEPS:
 * - generate an array of quotes
 * - run the program in the background
 * - keep an eye on whenever a new shell instance is opened
 * - if does, print a quote to __its stdout__ (until i figure out how to inject strings to a seperate stdout, ill use libnotify)
 */

int fd = -1;
int swd = -1;

void signal_handle(int sig){
  int close_status = -1;
  printf("Signal recieved, terminating.\n");
  close_status = inotify_rm_watch(fd, swd);

  if(close_status == -1)
    fprintf(stderr, "cwisdomgen: Failed to remove watch\n");

  close(fd);
  notify_uninit(); 
  exit(RET_SUCCESS);
}

int main(int argc, char** argv){

  srand(time(0));
  const struct inotify_event* watch_event;
  notify_init("CWisdomGen");

  // you can add any quotes you like here
  // make sure the wm/de you are using supports libnotify to see the notifications!
  char *quotes[] = {
    "\"When you have a dream, you've got to grab it and never let go.\" - Carol Burnett",
    "\"Nothing is impossible. The word itself says 'I'm possible!'\" - Audrey Hepburn",
    "\"There is nothing impossible to they who will try.\" - Alexander the Great",
    "\"The bad news is time flies. The good news is you're the pilot.\" - Michael Altshuler",
    "\"Life has got all those twists and turns. You've got to hold on tight and off you go.\" - Nicole Kidman",
  };
  int quotes_len = sizeof(quotes) / sizeof(quotes[0]);
  
  char *shell;
  
  if(getenv("SHELL"))
    shell = getenv("SHELL");
  else{
    fprintf(stderr, "cwisdomgen: Missing shell?\n");
    exit(RET_MISSING_SHELL);
  }

  fd = inotify_init();
  if (fd == -1){
    fprintf(stderr, "cwisdomgen: Failed to initialize inotify instance.\n");
    exit(RET_INOTIFY_INIT_FAIL);
  }

  swd = inotify_add_watch(fd, shell, IN_OPEN | IN_ACCESS);

  if (swd == -1){
    fprintf(stderr, "cwisdomgen: Failed to add shell to inotify watch instance.\n");
    exit(RET_INOTIFY_ADD_FAIL);
  }

  signal(SIGABRT, signal_handle);
  signal(SIGINT, signal_handle);
  signal(SIGTERM, signal_handle);
  
  char ebuf[4068];
  int len;
  
  while(true){
    len = read(fd, ebuf, 4096);

    if(len == -1){
      fprintf(stderr, "cwisdomgen: Failed to read from inotify event.\n");
      perror("read");
      exit(RET_INOTIFY_READ_FAIL);
    }

    // what the fuck is that last part
    for(char *ebufp = ebuf; ebufp<ebuf+len; ebufp+=sizeof( struct inotify_event ) + watch_event->len){
      watch_event = (const struct inotify_event*) ebufp;
      char *message = NULL;
      
      if(watch_event->mask & IN_OPEN)
	message = quotes[rand() % quotes_len];

      if(message){
	NotifyNotification * notif = notify_notification_new("CWisdomGen", message, "dialog-information");
	notify_notification_show(notif, NULL);
	g_object_unref(G_OBJECT(notif));
      }
      
    }
  }

}
