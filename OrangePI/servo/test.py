# Servo Control
import time
import wiringpi

# use 'GPIO naming'
wiringpi.wiringPiSetupGpio()

PIN=18

# set #18 to be a PWM output
wiringpi.pinMode(PIN, wiringpi.GPIO.PWM_OUTPUT)

# set the PWM mode to milliseconds stype
wiringpi.pwmSetMode(wiringpi.GPIO.PWM_MODE_MS)

# divide down clock
wiringpi.pwmSetClock(192)
wiringpi.pwmSetRange(2000)

delay_period = 0.01

while True:
    for pulse in range(50, 250, 1):
        wiringpi.pwmWrite(PIN, pulse)
        time.sleep(delay_period)
    for pulse in range(250, 50, -1):
        wiringpi.pwmWrite(PIN, pulse)
        time.sleep(delay_period)
    print(PIN)
