
# hpfan

A simple tool to control the fan speed of HP OMEN 17 (and possibly usable for other models).

# Installation

## From source

1. Clone the repository
2. Run `make build`
3. (Optional) Place `hpfan` in your `$PATH`, and maybe setuid it since it needs root privileges to write the pwm files.


# Usage

```shell
$ ./hpfan -h
Usage: ./hpfan [-t temp_file] [-p pwm_file] [-w temp_wall]
```

- `-t` specifies the file to read the temperature from. Defaults to `/sys/class/thermal/thermal_zone1/temp`.
- `-p` specifies the file to write the pwm value to. Defaults to `/sys/devices/platform/hp-wmi/hwmon/hwmon5/pwm1_enable`.
- `-w` specifies the threshold temperature to start the fan at. Defaults to `50000`, which is 50°C.

# License

MIT
