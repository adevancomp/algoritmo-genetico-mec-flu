#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE *gnuplotPipe = popen("gnuplot -persistent", "w");
    if (!gnuplotPipe) {
        fprintf(stderr, "Erro ao abrir o pipe do gnuplot.\n");
        return 1;
    }

    // Dados do gráfico
    double x[] = {1, 2, 3, 4, 5};
    double y[] = {2, 4, 6, 8, 10};
    int numPoints = sizeof(x) / sizeof(x[0]);

    // Comando para plotagem
    fprintf(gnuplotPipe, "plot '-' with lines\n");
    for (int i = 0; i < numPoints; ++i) {
        fprintf(gnuplotPipe, "%lf %lf\n", x[i], y[i]);
    }
    fprintf(gnuplotPipe, "e\n");

    fflush(gnuplotPipe);
    pclose(gnuplotPipe);

    return 0;
}
