# Исследование оптимизаций построения множества Мандельброта

**Автор**: Абрютин Иван Дмитриевич
**Дата**: 02.04.2025
**Курс**: 1, ИВТСП


## Изучение реализации без оптимизаций
Компилятор: MinGW GCC 13.1.0

Рассмотрим результаты компиляции при оптимизаций `-Og`, `-O2`, `-Ofast`

При компиляции с флагами `-Og -msse4.2 -mavx2 -DNDEBUG`


## Изучение реализации с массивной оптимизацией

Рассмотрим результаты компиляции с различными флагами

### `-Og -msse4.2 -mavx2 -DNDEBUG`


- Видно, код транслируется без значительных оптимизаций. Массивы и циклы по 4 остались в ассемблерном коде
```c++
for (int i = 0; i < 4; i++) {
    x_arr4[i] = x2_arr4[i] - y2_arr4[i] + x0_arr4[i];
    y_arr4[i] = 2 * xy_arr4[i] + y0_arr4[i];
}
```

```nasm
.L12:
        movsx   rax, ecx
        vmovss  xmm0, DWORD PTR 64[rsp+rax*4]
        vsubss  xmm0, xmm0, DWORD PTR 80[rsp+rax*4]
        vaddss  xmm0, xmm0, DWORD PTR [rsp+rax*4]
        vmovss  DWORD PTR 32[rsp+rax*4], xmm0
        vmovss  xmm0, DWORD PTR 96[rsp+rax*4]
        vaddss  xmm0, xmm0, xmm0
        vaddss  xmm0, xmm0, DWORD PTR 16[rsp+rax*4]
        vmovss  DWORD PTR 48[rsp+rax*4], xmm0
        add     ecx, 1
```

### `-O2 -msse4.2 -mavx2 -DNDEBUG`
- При `-O2` компилятор свел работу с массивами и циклами по 4 к работе с мультимедийными регистрами, что способствовало ускорению в ~4 раза
```c++
for (int i = 0; i < 4; i++) {
    x_arr4[i] = x2_arr4[i] - y2_arr4[i] + x0_arr4[i];
    y_arr4[i] = 2 * xy_arr4[i] + y0_arr4[i];
}
```
```nasm
vcmpltps        xmm3, xmm2, xmm4
vblendvps       xmm3, xmm4, xmm2, xmm3
vcmpltps        xmm2, xmm15, xmm4
vblendvps       xmm15, xmm4, xmm15, xmm2
```

### `-Ofast -msse4.2 -mavx2 -DNDEBUG`
- При `-Ofast` к моему удивлению производительность снизилась. Это связано с тем, что компилятор решил отказаться от мультимедийных регистров в пользу развертывания цикла и выполнения инструкций в приятной конвейеру независимой по данным последовательности инструкций


```c++
for (int i = 0; i < 4; i++) {
    x2_arr4[i] = x_arr4[i] * x_arr4[i];
    y2_arr4[i] = y_arr4[i] * y_arr4[i];
    abs_arr4[i] = x2_arr4[i] + y2_arr4[i];
}

for (int i = 0; i < 4; i++) {
    xy_arr4[i] = x_arr4[i] * y_arr4[i];
}

for (int i = 0; i < 4; i++) {
    x_arr4[i] = x2_arr4[i] - y2_arr4[i] + x0_arr4[i];
    y_arr4[i] = 2 * xy_arr4[i] + y0_arr4[i];
}
```
- Для упрощения читаемости примера, переименую переменные

```c++
for (int i = 0; i < 4; i++) {
    x2[i] = x[i] * x[i];
    y2[i] = y[i] * y[i];
    abs[i] = x2[i] + y2[i];
}

for (int i = 0; i < 4; i++) {
    xy[i] = x[i] * y[i];
}

for (int i = 0; i < 4; i++) {
    x[i] = x2[i] - y2[i] + x0[i];
    y[i] = 2 * xy[i] + y0[i];
}
```
- Наглядно видно, что инструкции 1-7, 8-14 можно исполнить параллельно
```nasm
1) vmulss  xmm5, xmm14, xmm14          ; x2[0] = x[0] * x[0]
2) xor     edi, edi
3) vmulss  xmm14, xmm0, xmm14          ; xy[0] = y[0] * x[0]
4) vmulss  xmm11, xmm13, xmm13         ; x2[1] = x[1] * x[1]
6) vmulss  xmm13, xmm13, xmm1          ; xy[1] = y[1] * x[1]
7) vmulss  xmm4, xmm0, xmm0            ; y2[0] = y[0] * y[0]

8) vmovss  DWORD PTR 12[rsp], xmm5     ; сохранение x2[0]
9) vmulss  xmm5, xmm1, xmm1            ; y2[1] = y[1] * y[1]
10) vmovss  xmm1, DWORD PTR 12[rsp]    ; xmm1 = x2[0]
11) vaddss  xmm14, xmm14, xmm14         ; y[0] = 2 * xy[0]
12) vmulss  xmm7, xmm9, xmm9            ; y2[2] = y[2] * y[2]
13) vmulss  xmm8, xmm12, xmm12          ; y2[3] = y[3] * y[3]
14) vaddss  xmm13, xmm13, xmm13         ; y[1] = 2 * xy[1]
и т.д.
```





## Изучение массивной оптимизации

MinGW gcc 13.1.0

### -Og -msse4.2 -mavx2 -DNDEBUG


### -O2 -msse4.2 -mavx2 -DNDEBUG
1) Работа с массивами из 4 float преобразуются в работу  с xmm и мультимедийными операциями. Обработка 4 числел происходит за одну операцию

2) Происходит разворачивание внутреннего цикла (по ix) в 4 параллельных. Происходит ускорение за счет конвейеризации

### -Ofast -msse4.2 -mavx2 -DNDEBUG

2) Происходит разворачивание внутреннего цикла (по ix) в 4 параллельных. Происходит ускорение за счет конвейеризации
