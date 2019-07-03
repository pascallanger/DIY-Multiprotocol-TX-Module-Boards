#!/bin/bash
set -e

AVR_VERSION=$(grep "^version=[0-9].[0-9].[0-9]" "avr/platform.txt" | awk -F = '{ print $2 }')
STM_VERSION=$(grep "^version=[0-9].[0-9].[0-9]" "stm32/platform.txt" | awk -F = '{ print $2 }')


echo "AVR Version: $AVR_VERSION"

# Check that the boards.txt version matches the platform version
grep -n "build.board=" avr/boards.txt | while read -r line ; do
    if [ $(echo $line | cut -d'=' -f3) -ne ${AVR_VERSION//./} ] 
    then
        echo "Incorrect version in avr/boards.txt:"
        echo "Line $line";
        echo
        exit 1
    fi
done

echo "Creating archive 'package_multi_4in1_avr_board_v$AVR_VERSION.tar.gz'"
tar -czf ../archives/package_multi_4in1_avr_board_v$AVR_VERSION.tar.gz --transform s/avr/package_multi_4in1_avr_board_v$AVR_VERSION/ avr/*
sleep 1s
echo
echo "Package: package_multi_4in1_avr_board_v$AVR_VERSION.tar.gz"
echo "SHA256:  `(sha256sum ../archives/package_multi_4in1_avr_board_v$AVR_VERSION.tar.gz | awk -v N=1 '{print $N}')`"
echo "Size:    `(ls -al ../archives/package_multi_4in1_avr_board_v$AVR_VERSION.tar.gz | awk -v N=5 '{print $N}')`"
echo


echo "STM Version: $STM_VERSION"

# Check that the boards.txt version matches the platform version
grep -n "build.board=" stm32/boards.txt | while read -r line ; do
    if [ $(echo $line | cut -d'=' -f3) -ne ${STM_VERSION//./} ] 
    then
        echo "Incorrect version in stm32/boards.txt:"
        echo "Line $line";
        echo
        exit 1
    fi
done

echo "Creating archive 'package_multi_4in1_stm32_board_v$STM_VERSION.tar.gz'"
tar -czf ../archives/package_multi_4in1_stm32_board_v$STM_VERSION.tar.gz --transform s/stm32/package_multi_4in1_stm32_board_v$STM_VERSION/ stm32/*
sleep 1s
echo
echo "Package: package_multi_4in1_stm_board_v$STM_VERSION.tar.gz"
echo "SHA256:  `(sha256sum ../archives/package_multi_4in1_stm32_board_v$STM_VERSION.tar.gz | awk -v N=1 '{print $N}')`"
echo "Size:    `(ls -al ../archives/package_multi_4in1_stm32_board_v$STM_VERSION.tar.gz | awk -v N=5 '{print $N}')`"
echo
