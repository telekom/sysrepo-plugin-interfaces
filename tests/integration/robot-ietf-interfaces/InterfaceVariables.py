
def getVariables():
    # Variables get imported into robot from the dict as Key = Value pairs
    variables = {
        'Supported If Types':  [
            'iana-if-type:softwareLoopback',
            'iana-if-type:ethernetCsmacd',
            'iana-if-type:l2vlan',
            'iana-if-type:other',
        ],
        'Expected Dummy': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>' \
                          '<name>dummy</name><type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">' \
                          'ianaift:other</type><enabled>true</enabled></interface></interfaces>',
    }

    return variables
