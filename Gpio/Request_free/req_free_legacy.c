#include<linux/module.h>
#include<linux/init.h>
#include<linux/gpio.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Rakshith GPIO");
MODULE_DESCRIPTION("GPIO Request Module");

#define GPIO_LED 0
#define GPIO_BUTTON 1

#define GPIO_OFFSET 0

static int led_gpio = (GPIO_LED + GPIO_OFFSET);
static int button_gpio = (GPIO_BUTTON + GPIO_OFFSET);

static int my_init(void)
{
    int status;
    status = gpio_request(led_gpio, "led_gpio");
    if(status)
    {
        pr_info("led gpio request faild , status : %d\n",status);
        return status;
    }

    gpio_direction_output(led_gpio, 0);

    status = gpio_request(button_gpio, "button_gpio");

    if(status)
    {
        pr_info("button gpio request failed\n");
        gpio_free(led_gpio);
        return status;
    }

    gpio_direction_input(button_gpio);

    gpio_set_value(led_gpio, 1);
    pr_info("Set led value to 1\n");

    pr_info("Status of button : %d\n", gpio_get_value(button_gpio));

    pr_info("GPIO example loaded\n");
    return 0;

}

static void my_exit(void)
{
    gpio_set_value(led_gpio,0);
    gpio_free(led_gpio);
    gpio_free(button_gpio);
    pr_info("GPIO example unloaded\n");
}

module_init(my_init);
module_exit(my_exit);