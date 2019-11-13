import os
from optparse import OptionParser

parser = OptionParser()
parser.add_option("-s", "--N_STRIDE_INT", dest="N_STRIDE_INT",
                  help="visit arr[i], then arr[i+N_STRIDE_INT]")
parser.add_option("-l", "--N_VISIT_PER_LOOP", dest="N_VISIT_PER_LOOP",
                  help="while (p < end) <unrolled accesses...>; p+=l;")

(options, args) = parser.parse_args()

N_STRIDE_INT = int(options.N_STRIDE_INT)
N_VISIT_PER_LOOP = int(options.N_VISIT_PER_LOOP)

def generate_loop_unrolling_rd():
    macro = "#define OP(i) sum += p[i];"
    ret = f"{macro}\n"

    for i in range(0, N_VISIT_PER_LOOP, N_STRIDE_INT):
        ret += f"  OP({i})"
    ret += f"\np += {N_VISIT_PER_LOOP};"

    return ret

def generate_loop_unrolling_wr():
    macro = "#define OP(i) p[i] = 1;"
    ret = f"{macro}\n"

    for i in range(0, N_VISIT_PER_LOOP, N_STRIDE_INT):
        ret += f"  OP({i})"
    ret += f"\np += {N_VISIT_PER_LOOP};"

    return ret

def prepare():
    src = os.path.join(os.path.dirname(__file__), "ram_bandwidth.c.template")
    dst = os.path.join(os.path.dirname(__file__), "ram_bandwidth.c")

    with open(src, "r") as f:
        code = f.read()
        code = code.replace("$UNROLLED_LOOP_RD", generate_loop_unrolling_rd())
        code = code.replace("$UNROLLED_LOOP_WR", generate_loop_unrolling_wr())

    with open(dst, "w") as f:
        f.write(code)

if __name__ == '__main__':
    prepare()
