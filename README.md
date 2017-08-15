# sunxi-light-driver
sunxi light, normal /standby/等指示操作。

--------------------------< 通用举例 >------------------------------------------
||A 0-31     B 32-63     C 64-95    D 96-127  以此类推... (每个字母是32个)
||
||	举个例子: gpio PD19  也就是 96+19= 115  即 (bank - 1) x 32 + offset
||
||
||
||	; port configuration:
||	; port_name = port:GPIO<mux><pull up/down><driver level><default value>
||	; mux: GPIO function configuration, 0-input, 1-output, 2+: special funciton for modules
||	; pull up/down: 1-pull up, 2-pull down
||	; driver level: 0,1,2,3. 2 is recommended
||	; default value: default output value when this IO is configured as an output IO
||
||
||	vip_dev0_flash_en        = port:PD20<1><default><default><1>
||	vip_dev0_flash_mode      = port:PH01<2><0><default><default>
||
||
||	gpio_set_value(ifx_dev->gpio.reset, 1);
----------------------------<通用举例>--------------------------------------------
