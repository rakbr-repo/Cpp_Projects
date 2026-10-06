#include<linux/init.h>
#include<linux/module.h>
#include<linux/gpio/consumer.h>
#include<linux/timer_types.h>
#include<linux/interrupt.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith");
MODULE_DESCRIPTION("IRQ GPIO");

#define DEBOUNCING 20
#define LED_GPIO 0
#define BUTTON_GPIO 1
#define GPIO_OFFSET 0

static unsigned int led_gpio = (LED_GPIO + GPIO_OFFSET);
static unsigned int button_gpio = (BUTTON_GPIO + GPIO_OFFSET);
static struct gpio_desc *led,*button;
static unsigned int irq_no;

static int last_button_state;
static struct timer_list debounce_timer;

static irqreturn_t button_isr(int irq, void  *dev_id)
{
    pr_info("GPIO interrupt occured\n");
    last_button_state = gpiod_get_value(button);
    mod_timer(&debounce_timer, jiffies + msec_to_jiffies(DEBOUNCING));
    return IRQ_HANDLED;
}

static void debounce_timer_callback(struct timer_list *t)
{   
    int state = gpiod_get_value(button);
    if(state == last_button_state){
        pr_info("Valid button press detected\n");
        gpiod_set_value(led, !gpiod_get_value(led));
    }
}

static int my_init(void)
{
    int status;
    led = gpio_to_desc(led_gpio);
    if(!led)
    {
        pr_info("led gpio failed to init\n");
        return -1;
    }

    button = gpio_to_desc(button_gpio);
    if(!button)
    {
        pr_info("button gpio failed to init\n");
        return -1;
    }

    status = gpiod_direction_output(led,0);
    if(!status)
    {
        pr_error("Could not set led val to off\n");
        return -status;
    }

    status = gpiod_direction_input(button);
    if(!status)
    {
        pr_error("Could input for button");
        return -status;
    }

    irq_no = gpio_to_irq(button);
    if(irq_no<0)
    {
        pr_error("Could not get irq number\n");
        return irq_no;
    }

    status = request_irq(irq_no, button_isr, IRQF_TRIGGER_FALLING,"btn_irq",NULL);
    if(status)
    {
        pr_error("IRQ Request failed\n");
        gpio_free(led_gpio);
        gpio_free(button_gpio);
        return -status;
    }

    pr_info("IRQ Reuest done\n");
    timer_setup(&debounce_timer, debounce_timer_callback, 0);
    pr_info("GPIO request loaded\n");
    return 0;
}

static void my_exit(void)
{
    gpiod_set_value(led, 0);
    free_irq(irq_number, NULL);
    del_timer_sync(&debounce_timer);
    pr_info("Goodbye Kernel\n");
}

module_init(my_init);
module_exit(my_exit);

