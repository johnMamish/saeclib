# Statically-Allocated Embedded C Container Library
#### saeclib, prounounced "sake-lib"

## Introduction
THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

This library contains a few, very basic C containers that can be used in embedded systems where dynamic allocation is undesirable.

This library is aimed at higher performance cortex-m microcontorllers, or even cortex-a microcontrollers if they're running an RTOS. saeclib tries to avoid anything too performant, but might slip in an unnecessary divide here and there; it assumes that multiplies are cheap. Don't use this in safety critical or medical grade code.

Every data structure comes in 3 basic flavors:
  - one that holds memory blocks of arbitrary size, but returns pointers to those memory blocks, not values.   (container names have no prefix, for instance `saeclib_circular_buffer_t`).
  - one that holds `uint8_t`  (container name prefixed with u8, for instance `saeclib_u8_circular_buffer_t`)
  - one that holds `void*`    (container name prefixed with vp, for instance `saeclib_vp_circular_buffer_t`)

These containers will all be in the same `.c/.h` files and will all hopefully have the same tests.

## Available containers
  - [ ] statically allocated array
    - [ ] variable-size element
    - [ ] u8
    - [ ] void*
  - [ ] circular buffer
    - [ ] variable-size element
    - [ ] u8
    - [ ] void*
  - [ ] stack
    - [ ] variable-size element
    - [ ] u8
    - [ ] void*
  - [ ] collection
    - [ ] variable-size element
    - [ ] u8
    - [ ] void*


## A sprinkling of syntactic aspertame
One thing that frustrates me about designing the API of this library is this: how can we avoid the syntactic clutter of always having the caller carve out a block (or even 2!) of statically allocated memory and then pass it into container initializers? This is pretty ugly, for instance:

```C
static uint8_t uart_tx_char_buffer_space[1024];
static char_buffer_t char_buffer;

// ...

char_buffer_t_initialize(&char_buffer, uart_tx_char_buffer_space, 1024);
```

It would be a lot nicer to be able to just do:

```C
char_buffer_t cb = char_buffer_t_statically_allocate(1024);
```

I have a little bit of syntactic sugar to do this exact thing... but it needs to be used with caution and moderation. Hence the name syntactic aspertame.

```C
typedef struct allocspace
{
	void* allocspace;
	int size;
	int elt_size;
} allocspace_t;

#define allocspace_t_statically_allocate(numel, elem_size) \
({ \
    allocspace_t a; \
    static uint8_t space[(numel) * (elem_size)]; \
    a.allocspace = space; \
    a.size = (numel); \
    a.elt_size = (elem_size); \
    a; \
})
```

so then the function call `allocspace_t a = allocspace_t_statically_allocate(10, sizeof(uint32_t));` automatically expands to:

```C
a = ({ \
    allocspace_t a; \
    static uint8_t space[10 * sizeof(uint32_t)]; \
    a.allocspace = space; \
    a.size = 10; \
    a.elt_size = sizeof(uint32_t); \
    a; \
});
```

The problem with this is that it hides the fact that it statically allocates memory. If you accidentally slip one of these into a function that's called more than once and returns its statically allocated containers (which wouldn't be that hard to do), you end up with 2 containers pointing to the same memory space. So this syntactic sugar should be used with caution.
