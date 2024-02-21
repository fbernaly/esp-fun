# Homekit LED Light

This project demostrate how to build an Apple Homekit compatible LED Light using an ESP32, a push button and the [HomeSpan library](https://github.com/HomeSpan/HomeSpan).

## Wiring up

Modify the wiring as needed.

<div align="center">
<img height="50%" width="50%" src="./circuit_led_light.png">
</div>

```c++
int LED_PIN = 14;
int PUSH_BUTTON_PIN = 33;
```

## References

- [HomeSpan library](https://github.com/HomeSpan/HomeSpan)
- [Introduction to PN2222](https://www.theengineeringprojects.com/2019/06/introduction-to-pn2222.html)
- [Cirkit Designer](https://zenluo.notion.site/Cirkit-Designer-Tutorials-4f5a47656b384709838eb5350596bf95)
