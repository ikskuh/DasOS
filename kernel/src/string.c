



char * strcpy(char * dst, const char *src)
{
    char * tmp = dst;
    while(((*tmp++) = (*src++)));
    return dst;
}

char *strcat(char * dst, const char *src)
{
    char * tmp = dst;
    while(*tmp++);
    tmp--;
    while(((*tmp++) = (*src++)));
    return dst;
}
