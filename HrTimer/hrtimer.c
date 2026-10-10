// #include <ctime>
#include<linux/module.h>
#include<linux/init.h>
#include<linux/hrtimer.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("Hr timer example");

static struct hrtimer my_timer;
u64 start_time;
static ktime_t period;
#define MAX_SLOTS 20
static u64 timestamps[MAX_SLOTS];
static int index;

static enum hrtimer_restart my_callback_func(struct hrtimer *timer)
{
    // u64 end_time = ktime_get_ns();
    // pr_info("Timer expiry after %lld\n", end_time - start_time);

    if(index < MAX_SLOTS)
    {
        timestamps[index++] = ktime_get_ns();
    }

    //The kernel does not know when you want it to fire next unless you update the timer's internal expires deadline. 
    // hrtimer_forward_now() is the tool that updates that deadline.
    hrtimer_forward_now(&my_timer, period);
    return HRTIMER_RESTART; //HRTIMER_NORESTART
}

static int my_init(void)
{
    hrtimer_setup(&my_timer, &my_callback_func, CLOCK_MONOTONIC , HRTIMER_MODE_REL);
    start_time = ktime_get_ns();
    period = ktime_set(1, 0);

    hrtimer_start(&my_timer, period, HRTIMER_MODE_REL);
    pr_info("Timer Started\n");
    return 0;
}

static void my_exit(void)
{
    hrtimer_cancel(&my_timer);

    for(int i=1;i<index;i++)
    {
        pr_info("Iteration %d : %lld\n", i, timestamps[i] - timestamps[i-1]);
    }
    pr_info("Timer Cancel\n");
}

module_init(my_init);
module_exit(my_exit);