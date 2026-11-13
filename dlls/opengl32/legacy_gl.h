/* WoW64 immediate-mode command batching. See COPYING.LIB for license terms. */
#ifndef __WINE_OPENGL32_LEGACY_GL_H
#define __WINE_OPENGL32_LEGACY_GL_H

/* Fixed-width, pointer-free wire format shared by the PE32 and Unix halves. */
#define GL_BATCH_CAPACITY 512
struct gl_batch_command { UINT code, value[4]; };
struct gl_batch_buffer
{
    UINT teb, count;
    BOOL flushing;
    struct gl_batch_command commands[GL_BATCH_CAPACITY];
};
enum gl_batch_opcode
{
    gl_batch_begin, gl_batch_end, gl_batch_vertex2, gl_batch_vertex3, gl_batch_vertex4,
    gl_batch_tex2, gl_batch_normal3, gl_batch_color3, gl_batch_color4,
    gl_batch_color4ub, gl_batch_multi2, gl_batch_multi2arb, gl_batch_multi2sgis
};

#if defined(__i386__) && defined(WINE_OPENGL_BATCHING)
/* Slots 2..5 are used by the pixel-format cache. This slot belongs to this DLL. */
#define GL_BATCH_TEB_SLOT 6
extern volatile LONG gl_batch_disabled;
extern void gl_batch_flush( struct gl_batch_buffer *buffer );
extern void gl_batch_disable(void);

static inline __attribute__((always_inline)) void gl_batch_barrier(void)
{
    struct gl_batch_buffer *buffer = NtCurrentTeb()->glReserved1[GL_BATCH_TEB_SLOT];
    if (buffer && buffer->count && !buffer->flushing) gl_batch_flush( buffer );
}

static inline __attribute__((always_inline)) struct gl_batch_buffer *gl_batch_current(void)
{
    TEB *teb = NtCurrentTeb();
    struct gl_batch_buffer *buffer = teb->glReserved1[GL_BATCH_TEB_SLOT];
    if (!buffer || buffer->flushing) return NULL;
    if (gl_batch_disabled || !teb->glCurrentRC)
    {
        if (buffer->count) gl_batch_flush( buffer );
        return NULL;
    }
    if (buffer->count == GL_BATCH_CAPACITY) gl_batch_flush( buffer );
    return gl_batch_disabled ? NULL : buffer;
}

static inline __attribute__((always_inline)) UINT gl_batch_float( GLfloat value )
{
    union { GLfloat f; UINT u; } bits = { .f = value };
    return bits.u;
}

static inline __attribute__((always_inline)) UINT gl_batch_word( const void *ptr, UINT index )
{
    UINT value;
    /* Constant-sized copies compile to integer loads, without aliasing or x87 conversion. */
    __builtin_memcpy( &value, (const char *)ptr + index * sizeof(value), sizeof(value) );
    return value;
}

static inline __attribute__((always_inline)) void gl_batch_append( struct gl_batch_buffer *buffer,
        UINT code, UINT a, UINT b, UINT c, UINT d )
{
    struct gl_batch_command *command = &buffer->commands[buffer->count];
    command->code = code;
    command->value[0] = a;
    command->value[1] = b;
    command->value[2] = c;
    command->value[3] = d;
    buffer->count++;
}

/* Every unbatched GL/WGL operation observes all earlier commands on its thread. */
#define UNIX_CALL( func, params ) (gl_batch_barrier(), WINE_UNIX_CALL( unix_ ## func, params ))
#define GL_BATCH_BARRIER() gl_batch_barrier()
#else
#define UNIX_CALL( func, params ) WINE_UNIX_CALL( unix_ ## func, params )
#define GL_BATCH_BARRIER() ((void)0)
#endif

#endif /* __WINE_OPENGL32_LEGACY_GL_H */
