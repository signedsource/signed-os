float pow(long number, long exp) {
    long result = number;
    while (exp > 1) {
        result = result * number;
        exp--;
    }

    return result;
}