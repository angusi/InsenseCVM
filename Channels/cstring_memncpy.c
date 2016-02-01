/*
 * cstring.c 
 *
 * to provide some common ops on strings in c
 *
 */

// Needed as C-language memcpy (expanded by compiler to direct load/store) 
// requires word allignment and marshalling code does not always adhere to 
// word allignment. Should note that the standard library version of memcpy 
// and strcpy do not require word allignment.
void memncpy(void *dest, void *src, unsigned size){
  unsigned i=0;
  while(i < size){
    ((char *)dest)[i] = ((char *)src)[i];
    i++;
  }
}
