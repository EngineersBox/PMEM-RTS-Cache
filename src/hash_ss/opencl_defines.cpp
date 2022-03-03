#ifndef clion_defines_cl // pragma once
#define clion_defines_cl

#define __kernel
#define __global
#define __local
#define __constant
#define __private

#define half float

struct float2 { float x, y; };
struct float3 { float x, y, z; };
struct float4 { float x, y, z, w; };

#define gentype float
gentype	clamp(gentype x, float minval, float maxval);
gentype	degrees(gentype radians);
gentype	max(gentype x, gentype y);
gentype	min(gentype x, gentype y);
gentype	mix(gentype x, gentype y, gentype a);
gentype	radians(gentype degrees);
gentype	sign(gentype x);
gentype	smoothstep(gentype edge0, gentype edge1, gentype x);
gentype	step(gentype edge, gentype x);
#undef gentype

enum cl_mem_fence_flags {
    CLK_LOCAL_MEM_FENCE,
    CLK_GLOBAL_MEM_FENCE
};
void barrier(cl_mem_fence_flags flags);

#define gentype float
#define gentypen float4
gentypen vload4(size_t offset, const gentype *p);
void vstore4(gentypen data, size_t offset, gentype *p);
void vstore4(gentypen data, size_t offset, gentype *p);
#undef gentypen
#undef gentype
float vload_half(size_t offset, const half *p);
float4 vload_half4(size_t offset, const half *p);
void vstore_half(float data, size_t offset, half *p);
void vstore_half4(float4 data, size_t offset, half *p);
float4 vloada_half4(size_t offset, const half *p);
void vstorea_half4(float4 data, size_t offset, half *p);

unsigned int get_work_dim();
size_t get_global_size(unsigned int dimindx);
size_t get_global_id(unsigned int dimindx);
size_t get_local_size(unsigned int dimindx);
size_t get_local_id(unsigned int dimindx);
size_t get_num_groups(unsigned int dimindx);
size_t get_group_id(unsigned int dimindx);
size_t get_global_offset(unsigned int dimindx);

#ifndef STATIC_KEYWORD
#define STATIC_KEYWORD static
#endif

#endif // pragma once