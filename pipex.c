#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

char *ft_strjoin(char *s1, char *s2);
char **ft_split(char *str, char c);
int ft_strncmp(char *s1, char *s2, int n);

int search_char(char *str, char c)
{
    int i;
    int count;

    i = 0;
    count = 0;
    while (str[i] != '\0')
    {
        if (str[i] == c)
            return (1);
        i++;
    }
    return (-1);
}

void ft_putstr(char *str, int fd)
{
    int i;

    i = 0;
    while (str[i])
    {
        write(fd, &str[i], 1);
        i++;
    }
}

char *ft_strndup(char *str, int i)
{
    int j;
    char *dest;

    j = 0;
    dest = malloc(sizeof(char) * (i + 1));
    if (!dest)
        return (NULL);
    while (str[j] && j < i)
    {
        dest[j] = str[j];
        j++;
    }
    dest[j] = '\0';
    return (dest);
}

char *get_env_by_name(char *str, char **env)
{
    char *path;
    char *dir;
    char *res;
    char *tmp;
    int i;

    i = 0;
    while (env[i] && ft_strncmp(env[i], "PATH=", 5))
    {
        i++;
    }
    if (!env[i])
        return (str);
    path = &env[i][5];
    while (path && search_char(path, ':') != -1)
    {
        dir = ft_split(path, ':')[0];
        tmp = ft_strjoin(dir, "/");
        res = ft_strjoin(tmp, str);
        free(dir);
        free(tmp);
        if (access(res, F_OK) == 0)
            return (res);
        free(res);
        path = &path[search_char(path, ':') + 1];
    }
    return (NULL);
}

int ft_strlen(char *str)
{
    int i;

    i = 0;
    while (str[i])
        i++;
    return (i);
}

char *ft_strjoin(char *s1, char *s2)
{
    int i;
    int j;
    char *dest;

    i = 0;
    j = 0;
    dest = malloc(sizeof(char) * (ft_strlen(s1) + ft_strlen(s2) + 1));

    if (!dest || !s1 || !s2)
        return (NULL);
    while (s1[i])
    {
        dest[i] = s1[i];
        i++;
    }
    while (s2[j])
    {
        dest[i + j] = s2[j];
        j++;
    }
    dest[i + j] = '\0';
    return (dest);
}

void execute_cmd(char *cmd, char **env)
{
    char **args;
    char *path;

    args = ft_split(cmd, ' ');
    if (search_char(args[0], '/') != -1)
        path = args[0];
    else
        path = get_env_by_name(args[0], env);
    if (path)
        ft_putstr(path, 2);
    if (execve(path, args, env) == -1)
        ft_putstr("execve : commande not found", 2);
    exit(1);
}

char *ft_strdup(char *src)
{
    int i;
    char *dest;

    i = 0;
    dest = malloc(sizeof(char) * ft_strlen(src) + 1);
    while (src[i])
    {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
    return (dest);
}

int ft_strncmp(char *s1, char *s2, int n)
{
    int i;

    i = 0;
    while (s1[i] && s2[i] && i < n)
    {
        if (s1[i] != s2[i])
            return (s1[i] - s2[i]);
        i++;
    }
    return (0);
}

//access function
int open_file(char *filename, int in_out)
{
    int fd;
    if (in_out)
    {
        if (access(filename, F_OK) == 0)
        {
            fd = open(filename, O_RDONLY);
            return (fd);
        }
        else
        {
            ft_putstr("File doesn't exist\n", 2);
            exit(1);
        }
    }
    else
    {
        return (open(filename, O_CREAT | O_WRONLY | O_TRUNC,
                     S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH));
    }
}

void pipex(char *cmd1, int f1, int f2, char **env)
{
    pid_t pid;
    int p[2];

    dup2(f1, 0);
    dup2(f2, 1);
    pipe(p);
    pid = fork();
    if (pid == 0)
    {
        close(p[0]);
        dup2(p[1], 1);
        if (f1 == 0)
            exit(1);
        else
            execute_cmd(cmd1, env);
    }
    else
    {
        close(p[1]);
        dup2(p[0], 0);
        waitpid(pid, NULL, 0);
    }
}

char *ft_substr(char *str, int start, int len)
{
    int i;
    char *dest;

    i = 0;
    dest = malloc(sizeof(char) * (len + 1));

    if (!dest)
        return (NULL);
    while (str[start] && i < len)
    {
        dest[i] = str[start];
        start++;
        i++;
    }
    dest[i] = '\0';
    return (dest);
}

char **ft_split(char *str, char c)
{
    int i;
    int j;
    int k;
    char **tab;

    i = 0;
    j = 0;
    k = 0;
    tab = malloc(sizeof(char *) * (ft_strlen(str) + 1));

    if (!tab)
        return (NULL);
    while (str[i])
    {
        if (str[i] == c)
        {
            tab[j] = ft_substr(str, k, i - k);
            j++;
            k = i + 1;
        }
        i++;
    }
    tab[j] = ft_substr(str, k, i - k);
    tab[j + 1] = NULL;
    return (tab);
}

int main(int ac, char **ag, char **env)
{
    int f1;
    int f2;
    if (ac == 5)
    {
        f1 = open_file(ag[1], 1);
        f2 = open_file(ag[4], 0);
        pipex(ag[2], f1, f2, env);
        execute_cmd(ag[3], env);
    }
    else
    {
        ft_putstr("Usage: ./pipex <file1> <command1> <command2> <file2>\n", 2);
    }
    return (0);
}