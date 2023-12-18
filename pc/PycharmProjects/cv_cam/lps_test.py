import time

lps = 0
lps_last_time = 0
lps_print_sec = 10 #sec

while True:
    current_millis = round(time.time() * 1000)

    if ((current_millis - lps_last_time) > (lps_print_sec * 1000)):
        lps_last_time = round(time.time() * 1000)
        print("LPS: {0}".format(lps))
        lps = 0

    lps += 1