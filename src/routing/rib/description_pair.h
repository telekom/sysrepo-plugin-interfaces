/*
 * telekom / sysrepo-plugin-interfaces
 *
 * This program is made available under the terms of the
 * BSD 3-Clause license which is available at
 * https://opensource.org/licenses/BSD-3-Clause
 *
 * SPDX-FileCopyrightText: 2022 Deutsche Telekom AG
 * SPDX-FileContributor: Sartura Ltd.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef ROUTING_RIB_DESCRIPTION_PAIR_HPP
#define ROUTING_RIB_DESCRIPTION_PAIR_HPP

#define ROUTING_RIB_DESCRIPTION_SIZE 256

struct rib_description_pair {
	// name = 32, ipv4/ipv6 + '-' = 5
	char name[32 + 5];

	// 256 chosen for now, can be changed later
	char description[ROUTING_RIB_DESCRIPTION_SIZE];
};

#endif // ROUTING_RIB_DESCRIPTION_PAIR_HPP
