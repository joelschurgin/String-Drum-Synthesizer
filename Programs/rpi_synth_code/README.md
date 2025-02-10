# USEFUL COMMANDS #

# set alt functions (uses wiringPi pins)
gpio -g mode PIN_NUM alt0

# view alt functions for range of pins (uses BCM pins)
pinctrl funcs PinA-PinB

# view pinout and current alt functions
gpio readall