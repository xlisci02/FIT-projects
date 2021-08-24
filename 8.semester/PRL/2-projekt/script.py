import math

def get_proc_num_val(numvals, numprocs, rank):
    if ((numvals % numprocs) == 0):
        return numvals/numprocs
    chunk_size = int(math.ceil((float(numvals)/numprocs)))
    return chunk_size if (rank < (numvals % numprocs)) else chunk_size-1

def get_proc_num_val1(numvals, numprocs, rank):
    if ((numvals % numprocs) == 0):
        return numvals/numprocs
    chunk_size = int(math.floor((numvals/numprocs)))
    return chunk_size+1 if (rank < (numvals % numprocs)) else chunk_size


def find_less_pow_of_two(numvals):
    return (int(math.ceil(math.pow(2,math.floor(math.log(numvals,2))))))


MERLIN_LIMIT = 250
def get_num_of_proc_new(numvals):
    return ((len([ numvals - N*math.log(N,2) for N in range(1,numvals+1) if numvals - N*math.log(N,2) >= 0])))


# print(math.log(2,2))

for numvals in range(1,180):
#     # np1 = get_num_of_proc(numvals)
    np2 = get_num_of_proc_new(numvals)
    np2_orig = find_less_pow_of_two(np2)
#     # if np1 != np2:
    print("numvals:", numvals, "optimal:", np2, "result:", np2_orig)
    # for proc_id in range(0,np):
    print(int(math.pow(2,math.floor(math.log(min((len([ numvals - N*math.log(N,2) for N in range(1,numvals+1) if numvals - N*math.log(N,2) >= 0])), MERLIN_LIMIT),2)))))


    #     a = get_proc_num_val(numvals,np,proc_id)
    #     b = get_proc_num_val1(numvals,np,proc_id)
    #     if a != b:
    #         print(a,b, numvals, np, proc_id)
