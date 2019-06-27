#!/bin/bash

# Get the details from the latest AVR package
AVR_VERSION=$(grep "^version=[0-9].[0-9].[0-9]" "avr/platform.txt" | awk -F = '{ print $2 }')
AVR_CHECKSUM=`(sha256sum ../archives/package_multi_4in1_avr_board_v$AVR_VERSION.tar.gz | awk -v N=1 '{print $N}')`
AVR_SIZE=`(ls -al ../archives/package_multi_4in1_avr_board_v$AVR_VERSION.tar.gz | awk -v N=5 '{print $N}')`

# Pull in the AVR template and updated it for the new package
AVR_TEMPLATE=$(<json_template_avr.json)
AVR_TEMPLATE=${AVR_TEMPLATE//\{version\}/$AVR_VERSION}
AVR_TEMPLATE=${AVR_TEMPLATE//\{checksum\}/$AVR_CHECKSUM}
AVR_TEMPLATE=${AVR_TEMPLATE//\{size\}/$AVR_SIZE}

# Get the details from the latest STM package
STM_VERSION=$(grep "^version=[0-9].[0-9].[0-9]" "stm32/platform.txt" | awk -F = '{ print $2 }')
STM_CHECKSUM=`(sha256sum ../archives/package_multi_4in1_stm32_board_v$STM_VERSION.tar.gz | awk -v N=1 '{print $N}')`
STM_SIZE=`(ls -al ../archives/package_multi_4in1_stm32_board_v$STM_VERSION.tar.gz | awk -v N=5 '{print $N}')`

# Pull in the STM template and updated it for the new package
STM_TEMPLATE=$(<json_template_stm32.json)
STM_TEMPLATE=${STM_TEMPLATE//\{version\}/$STM_VERSION}
STM_TEMPLATE=${STM_TEMPLATE//\{checksum\}/$STM_CHECKSUM}
STM_TEMPLATE=${STM_TEMPLATE//\{size\}/$STM_SIZE}

# Strings which mark the point where we want to insert the new sections
AVR_SEARCH_STRING=`echo -e "      },\n      {\n        \"name\": \"Multi 4-in-1 STM32 Board\","`
STM_SEARCH_STRING=`echo -e "      },\n      {\n        \"name\": \"Multi 4-in-1 OrangeRX Board - DEPRECATED, USE MULTI 4-IN-1 AVR BOARDS PACKAGE INSTEAD\","`

# Insert the new sections in the master JSON file
MASTER_JSON=$(<../package_multi_4in1_board_index.json)
MASTER_JSON=${MASTER_JSON/$AVR_SEARCH_STRING/$AVR_TEMPLATE}
MASTER_JSON=${MASTER_JSON/$STM_SEARCH_STRING/$STM_TEMPLATE}

# Update the templates with URLs for the devel JSON file
AVR_TEMPLATE=${AVR_TEMPLATE//\/raw\/master\/archives\//\/raw/devel\/archives\/}
STM_TEMPLATE=${STM_TEMPLATE//\/raw\/master\/archives\//\/raw/devel\/archives\/}

# Insert the new sections in the devel JSON file
DEVEL_JSON=$(<package_multi_4in1_board_devel_index.json)
DEVEL_JSON=${DEVEL_JSON/$AVR_SEARCH_STRING/$AVR_TEMPLATE}
DEVEL_JSON=${DEVEL_JSON/$STM_SEARCH_STRING/$STM_TEMPLATE}

# Output the updated JSON files
echo "$MASTER_JSON">../package_multi_4in1_board_index.json
echo "$DEVEL_JSON">package_multi_4in1_board_devel_index.json
