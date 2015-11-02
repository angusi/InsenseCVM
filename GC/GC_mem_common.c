/*
 * Memory-management functions
 *
 * Copyright (c) 2015, Angus Ireland
 * School of Computer Science, St. Andrews University
 *
 * A simplified version of memory management functions from the Insense Runtime for Unix.
 *
 * Copyright (c) 2013, Jonathan Lewis, Pavlo Bazilinskyy <pavlo.bazilinskyy@gmail.com>
 * School of Computer Science, St. Andrews University
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */


#include "GC_mem.h"
#include "../Logger.h"
#include "Strings.h"

// mutex to serialise memory operation when using a shared heap
pthread_mutex_t* GC_mutex;

void GC_init() {
    GC_mutex = malloc(sizeof(pthread_mutex_t));
    pthread_mutex_init(GC_mutex, NULL);
}

void GC_assign(void *generic_var_pntr, void *new_mem) {

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, GARBAGE_COLLECTOR_NAME, GARBAGE_COLLECTOR_ASSIGN, new_mem, generic_var_pntr);
#endif

    void **var_pntr = (void **) generic_var_pntr;
    void *old_mem = *var_pntr; // can deref void** but not void*

    if(old_mem == new_mem) {
        // If old memory address is same as the new one don't do anything
        return;
    }

    if(old_mem != NULL) {
        // If old memory is not null update ref_count and free if 0
        GC_decRef(old_mem);
    }
    if(new_mem != NULL) {
        // if new memory is not null
        GC_incRef(new_mem);
    }

    // complete the assignment of memory address to caller's variable
    *var_pntr = new_mem; // as var_pntr has type void** can dereference it here
}

/**
 * Allocate space in memory for a new, garbage collected object.
 * The memory is automatically zero'd.
 *
 * @param[in] size The size, in bytes, to allocate
 * @param[in] mem_contains_pointers If this memory will contain pointers, set to true.
 */
void* GC_alloc(size_t size, bool mem_contains_pointers){
#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, GARBAGE_COLLECTOR_NAME, GARBAGE_COLLECTOR_ALLOCATING_BYTES, size);
#endif
	//Allocate memory (required memory + GC overhead)
	void* new_memory = malloc(size + sizeof(GC_Header_s));

    if(new_memory == NULL) {
        log_logMessage(ERROR, GARBAGE_COLLECTOR_NAME, GARBAGE_COLLECTOR_OOM);
        return NULL;
    }

	// zero memory area to avoid having to set all pointer types to NULL
	memset(new_memory, 0, (size + sizeof(GC_Header_s)));
	GC_Header_PNTR header = ((GC_Header_PNTR) new_memory);
	header->ref_count = 0;
	header->mem_contains_pointers = mem_contains_pointers;
	header->mutex = GC_mutex;

	//Only return the required memory
	return (new_memory + sizeof(GC_Header_s));
}

/*
 * Decrements references to a given memory object.
 * If the object has no references left, also garbage collects it.
 *
 * @param[in] pntr Pointer to object to decrement references to.
 */
void GC_decRef(void* pntr) {

	if(pntr==NULL){
        log_logMessage(WARNING, GARBAGE_COLLECTOR_NAME, GARBAGE_COLLECTOR_DECREF_NULL);
		return;
	}

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, GARBAGE_COLLECTOR_NAME, "Decrementing reference count");
#endif
	GC_Header_PNTR header = (GC_Header_PNTR) (pntr - sizeof(GC_Header_s));
	pthread_mutex_lock(header->mutex);
	header->ref_count -= 1;

    // If the memory is now not pointed to by anything, free it.
	if(header->ref_count <= 0) {
        //If the memory containers pointers to other objects, decrement their reference counts first.
		if(header->mem_contains_pointers) {
            //This is done by casting to a GC_Container_PNTR and calling the first field, the decRef method.
			GC_Container_PNTR thisMem = (GC_Container_PNTR) pntr;
			thisMem->decRef(pntr);
#ifdef DEBUGGINGENABLED
			log_logMessage(DEBUG, GARBAGE_COLLECTOR_NAME, "Decrementing contained pointers");
#endif
		}
		GC_free(pntr);
	}
    pthread_mutex_unlock(header->mutex);
}

/*
 * Increments reference count for memory referenced by pntr.
 */
void GC_incRef(void *pntr) {
    if(pntr==NULL){
        log_logMessage(WARNING, GARBAGE_COLLECTOR_NAME, "Ignoring call to increment NULL pointer references");
        return;
    }

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, GARBAGE_COLLECTOR_NAME, "Incrementing reference count");
#endif
    GC_Header_PNTR header = (GC_Header_PNTR) (pntr - sizeof(GC_Header_s));
    pthread_mutex_lock(header->mutex);
    header->ref_count += 1;
    pthread_mutex_unlock(header->mutex);
}

/*
 * Frees memory assigned by DAL_alloc and referenced by pntr.
 */
void GC_free(void *pntr){

	if(pntr==NULL){
        log_logMessage(WARNING, GARBAGE_COLLECTOR_NAME, "Ignoring call to free NULL pointer");
        return;
	}

	GC_Header_PNTR header = (GC_Header_PNTR) (pntr - sizeof(GC_Header_s));
	if(header->ref_count > 0){
        log_logMessage(ERROR, GARBAGE_COLLECTOR_NAME, "Cannot free memory object that is still referenced.");
		return;
	}

#ifdef DEBUGGINGENABLED
    log_logMessage(DEBUG, GARBAGE_COLLECTOR_NAME, "Freeing memory");
#endif
	free(header);
}
