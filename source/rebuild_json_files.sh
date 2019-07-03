#!/bin/bash

JSON_HEADER="{
  \"packages\": [{
    \"name\": \"multi4in1\",
    \"maintainer\": \"Pascal Langer\",
    \"websiteURL\": \"https://github.com/pascallanger/DIY-Multiprotocol-TX-Module\",
    \"email\": \"pascal_langer@yahoo.fr\",
    \"help\": {
      \"online\": \"https://github.com/pascallanger/DIY-Multiprotocol-TX-Module\"
    },
    \"platforms\": ["

JSON_FOOTER="
      {
        \"name\": \"Multi 4-in-1 OrangeRX Board - DEPRECATED, USE MULTI 4-IN-1 AVR BOARDS PACKAGE INSTEAD\",
        \"architecture\": \"orangerx\",
        \"version\": \"1.0.1\",
        \"category\": \"Contributed\",
        \"help\": {
          \"online\": \"https://github.com/pascallanger/DIY-Multiprotocol-TX-Module\"
        },
        \"url\": \"https://github.com/pascallanger/DIY-Multiprotocol-TX-Module-Boards/raw/master/archives/package_multi_4in1_orangerx_board_v1.0.1.tar.gz\",
        \"archiveFileName\": \"package_multi_4in1_orangerx_board_v1.0.1.tar.gz\",
        \"checksum\": \"SHA-256:7287ce61028b754bb8ff947317dd15773fc7eeecd752826c707fa356b9b36dc6\",
        \"size\": \"161615\",
        \"boards\": [{
          \"name\": \"Multi 4-in-1 (OrangeRX)\"
        }],
        \"toolsDependencies\": []
      }
    ],
    \"tools\": []
  }]
}"

JSON_OUTPUT=$JSON_HEADER

# Parse the AVR Files
ARCHIVES=../archives/package_multi_4in1_avr*
for FILE in $ARCHIVES
do
    VERSION=$(echo $FILE | sed -r 's/.*v([0-9]+\.[0-9]+\.[0-9]+).*/\1/')
    CHECKSUM=$(sha256sum $FILE | awk -v N=1 '{print $N}')
    SIZE=$(ls -al $FILE | awk -v N=5 '{print $N}')

    TEMPLATE=$(<json_template_avr)
    TEMPLATE=${TEMPLATE//\{file\}/$(basename $FILE)}
    TEMPLATE=${TEMPLATE//\{version\}/$VERSION}
    TEMPLATE=${TEMPLATE//\{checksum\}/$CHECKSUM}
    TEMPLATE=${TEMPLATE//\{size\}/$SIZE}

    JSON_OUTPUT="$JSON_OUTPUT$TEMPLATE"
done

# Parse the STM32 Files
ARCHIVES=../archives/package_multi_4in1_stm32*
for FILE in $ARCHIVES
do
    VERSION=$(echo $FILE | sed -r 's/.*v([0-9]+\.[0-9]+\.[0-9]+).*/\1/')
    CHECKSUM=$(sha256sum $FILE | awk -v N=1 '{print $N}')
    SIZE=$(ls -al $FILE | awk -v N=5 '{print $N}')

    TEMPLATE=$(<json_template_stm32)
    TEMPLATE=${TEMPLATE//\{file\}/$(basename $FILE)}
    TEMPLATE=${TEMPLATE//\{version\}/$VERSION}
    TEMPLATE=${TEMPLATE//\{checksum\}/$CHECKSUM}
    TEMPLATE=${TEMPLATE//\{size\}/$SIZE}

    JSON_OUTPUT="$JSON_OUTPUT$TEMPLATE"
done

# Add the footer
JSON_OUTPUT="$JSON_OUTPUT$JSON_FOOTER"

# Output the main JSON file
echo "$JSON_OUTPUT">../package_multi_4in1_board_index.json

# Update the JSON for the devel file
JSON_OUTPUT=${JSON_OUTPUT//\"name\": \"multi4in1\"/\"name\": \"multi4in1-devel\"}
JSON_OUTPUT=${JSON_OUTPUT//\/raw\/master\/archives\//\/raw/devel\/archives\/}
JSON_OUTPUT=${JSON_OUTPUT//Multi 4-in-1 AVR Boards/Multi 4-in-1 AVR Boards (Devel)}
JSON_OUTPUT=${JSON_OUTPUT//Multi 4-in-1 STM32 Board/Multi 4-in-1 STM32 Board (Devel)}

# Output the devel JSON file
echo "$JSON_OUTPUT">package_multi_4in1_board_devel_index.json
