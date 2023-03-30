/* Changes by NEC Corporation for the VE port, 2020 */

struct link_map_machine
  {
    Elf64_Addr plt; /* Address of .plt + 0x16 */
    Elf64_Addr gotplt; /* Address of .got + 0x18 */
    void *tlsdesc_table; /* Address of TLS descriptor hash table.  */
  };
