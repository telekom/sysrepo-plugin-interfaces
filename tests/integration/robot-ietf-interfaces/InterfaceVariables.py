
def getVariables():
    # Variables get imported into robot from the dict as Key = Value pairs
    variables = {
        'Supported If Types':  [
            'iana-if-type:softwareLoopback',
            'iana-if-type:ethernetCsmacd',
            'iana-if-type:l2vlan',
            'iana-if-type:other',
            'iana-if-type:bridge',
        ],
        'Expected Dummy': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>' \
                          '<name>dummy</name><type xmlns:ianaift="urn:ietf:params:xml:ns:yang:iana-if-type">' \
                          'ianaift:other</type><enabled>true</enabled></interface></interfaces>',
                          
        'Expected Ip Prefix Address': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>' \
                                      '<name>lo</name><ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">' \
                                      '<address><ip>127.0.0.0</ip><prefix-length>8</prefix-length></address></ipv4></interface></interfaces>',

        'Expected Ip Netmask Address': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>' \
                                       '<name>lo</name><ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">' \
                                       '<address><ip>127.0.0.0</ip><netmask>255.0.0.0</netmask></address></ipv4></interface></interfaces>',

        'Expected Ip Sub Address': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces"><interface>' \
                                   '<name>lo</name><ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">' \
                                   '<address><ip>::1</ip><prefix-length>128</prefix-length></address></ipv6></interface></interfaces>',
        'Expected IPv4 Mtu': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">' \
                        '<interface><name>lo</name><ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">' \
                        '<mtu>12345</mtu></ipv4></interface></interfaces>',
        'Expected IPv6 Mtu': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">' \
                             '<interface><name>lo</name><ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">' \
                             '<mtu>1300</mtu></ipv6></interface></interfaces>',
        'Expected IPv4 Forwarding': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">' \
                                    '<interface><name>lo</name><ipv4 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">' \
                                    '<forwarding>true</forwarding></ipv4></interface></interfaces>',
        'Expected IPv6 Forwarding': '<interfaces xmlns="urn:ietf:params:xml:ns:yang:ietf-interfaces">' \
                                    '<interface><name>lo</name><ipv6 xmlns="urn:ietf:params:xml:ns:yang:ietf-ip">' \
                                    '<forwarding>true</forwarding></ipv6></interface></interfaces>',
    }

    return variables
