# `esphome-component-ping`

A custom `esphome` component that sends and receives `ICMP`.

This is my first attempt to learn `esphome` custom sensor component.

I have almost zero `C++` knowledge. You have been warned.

## Bugs

* the target is hard-coded
* various configuration macros are hard-coded
* the target must be IP address
* multiple targets are not supported

## Requirements

* `python` 3.8
* `pipenv` (only if you do not have a working `esphome` installation)

## Usage

If you do not have a working `esphome`, install it. Or, install it with
[`pipenv`](https://pipenv.pypa.io/en/latest/).

```console
pipenv install
```

Run your shell in the python virtual environment.

```console
pipenv shell
```

Copy `secrets.yaml`. You probably want to modify it, i.e. password, `SSID`,
etc.

```console
cp config/secrets.yaml.dist config/secrets.yaml
```

Compile `ping.yaml` in the python virtual environment.

```console
esphome compile config/ping.yaml
```
