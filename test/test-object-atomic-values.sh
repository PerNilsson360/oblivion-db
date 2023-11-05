#! /bin/bash
function test() {
    echo '{}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"a":null,"b":true,"c":10,"d":3.14,"e":"foo"}' | \
        ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"c":0}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"c":100}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 0 ]; then
        return 255
    fi
    echo '{"c":-1}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 255 ]; then
        return 255
    fi
    echo '{"c":101}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 255 ]; then
        return 255
    fi
    echo '{"c":11}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 255 ]; then
        return 255
    fi
    echo '{"a":true}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 255 ]; then
        return 255
    fi
    echo '{"z":true}' | ../src/jsch --schema=object-atomic-values-schema.json
    if [ $? -ne 255 ]; then
        return 255
    fi

    return 0
}
test
