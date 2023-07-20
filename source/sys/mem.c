//#include <stdio.h>
#include <string.h>
#include <FreeRTOS.h>

// the functions available here are ment to be included in heap_4.c

//this definiton is the same inside heap_4.c
typedef struct A_BLOCK_LINK
{
	struct A_BLOCK_LINK *pxNextFreeBlock;	/*<< The next free block in the list. */
	size_t xBlockSize;						/*<< The size of the free block. */
} BlockLink_t;

/* The size of the structure placed at the beginning of each allocated memory
block must by correctly byte aligned. */
static const size_t xHeapStructSize	= ( sizeof( BlockLink_t ) + ( ( size_t ) ( portBYTE_ALIGNMENT - 1 ) ) ) & ~( ( size_t ) portBYTE_ALIGNMENT_MASK );

#ifndef heapBITS_PER_BYTE
#define heapBITS_PER_BYTE		( ( size_t ) 8 )
#endif

static size_t xBlockAllocatedBit=( ( size_t ) 1 ) << ( ( sizeof( size_t ) * heapBITS_PER_BYTE ) - 1 );

void *pvPortCalloc (size_t num, size_t size)
{
    uint32_t i,j;
    uint8_t *pointer;
    
    pointer=pvPortMalloc(num*size);
    
    if(pointer!=NULL)
    {
    for(i=0;i<num;i++)
        {
        for(j=0;j<size;j++)
            {
            pointer[i*size+j]=0;
            }
        }
    }
    
    return (void*) pointer;
}

//function from forum community

void *pvPortRealloc( void *pv, size_t xWantedSize )
{	
	size_t move_size;
	size_t block_size;
	BlockLink_t *pxLink;
	void *pvReturn = NULL;
	uint8_t *puc = ( uint8_t * ) pv;

	//Exit on NULL
	if (xWantedSize > 0)
	{
		// Check if buffer is valid
		if (pv != NULL)
		{
			// Memory being freed will have an BlockLink_t structure immediately before it.
			puc -= xHeapStructSize;

			// Casting is to keep the compiler from issuing warnings.
			pxLink = ( void * ) puc;

			// Check allocate block
			if ((pxLink->xBlockSize & xBlockAllocatedBit) != 0)
			{
				// The block is being returned to the heap - it is no longer allocated.
				block_size = (pxLink->xBlockSize & ~xBlockAllocatedBit) - xHeapStructSize;

				// New memory space allocation
				pvReturn = pvPortCalloc(1, xWantedSize);

				// Check creation
				if (pvReturn != NULL)
				{
					// Move only to the limit
					if (block_size < xWantedSize)
					{
						// New space available is less than before
						move_size = block_size;
					}
					else
					{
						// New space available is more than before
						move_size = xWantedSize;
					}

					// Data copy into new memory space
					memcpy(pvReturn, pv, move_size);

					// Freeing unused memory space
					vPortFree(pv);
				}
			}
			else
			{
				// Null pointer, allocate new memory
				pvReturn = pvPortCalloc(1, xWantedSize);
			}
		}
		else
		{
			// Null pointer, allocate new memory
			pvReturn = pvPortCalloc(1, xWantedSize);
		}
	}
	else
	{
		// Exit without memory block
		pvReturn = NULL;
	}

	// Exit with memory block
	return pvReturn;
}

void *__wrap_malloc (size_t size)
{
    return pvPortMalloc(size);
}

void *__wrap_calloc (size_t num, size_t size)
{
    return pvPortCalloc(num,size);
}

//void *__wrap_realloc (void *, size_t);
void __wrap_free (void * pointer)
{
    vPortFree(pointer);
}

void *__wrap_realloc( void *ptr, size_t new_size )
{
    return pvPortRealloc(ptr, new_size);
}
