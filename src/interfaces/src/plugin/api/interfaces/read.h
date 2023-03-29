/*
 * telekom / sysrepo-plugin-system
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
#ifndef READ_H
#define READ_H

int read_from_proc_file(const char* dir_path, char* interface, const char* fn, int* val);

int read_from_sys_file(const char* dir_path, char* interface, int* val);

#endif /* READ_H */
