#include <stdio.h>
#include <stdlib.h>
#include "sacio.h"

void no_spa(char *ps){
        char *pt = ps;
        while ( *ps != '\0'  ){
            if ( *ps != ' ' && *ps != '\n'  ){
                *pt++ = *ps;
            }
            ++ps;
        }
        *pt = '\0';
}


int main(int argc, char *argv[]) {
    int i, j, npts;
    float *data, **amp, dist, time, b, e, delta, distmin = 180., distmax = 0.;
    char ss[256];
    SACHEAD hd;
    FILE *f, *fout, *fp;

    if( argc != 3 ) {
        fprintf(stderr,"Usage: dist-time sacfile outfile_name\n");
        fprintf(stderr,"       return \"outfile_name\" saving time-distance info\n");
        fprintf(stderr,"       <and \"plot.sh\" will be saved to plot time-distance\n");
        fprintf(stderr,"       just execute \"sh plot.sh\" to plot time-disance power\n");
        fprintf(stderr,"       Attention!!! Executing \"sh plot.sh\" requires GMT(the Generic Mapping Tools)\n");
        exit(1);
    }

    f = fopen(argv[1],"r");
    fout = fopen(argv[2],"w");
    fp = fopen("plot.sh","w");

    while ( fgets(ss,256,f) ) {
        no_spa(ss);
        data = read_sac(ss,&hd);
        npts = hd.npts;
        b = hd.b;
        e = hd.e;
        delta = hd.delta;
        dist = hd.gcarc;
        if ( hd.gcarc > distmax ) distmax = hd.gcarc;
        if ( hd.gcarc < distmin ) distmin = hd.gcarc;
        for ( i = 0; i < npts; i ++ ) {
            time = i*delta;
            fprintf(fout,"%f %f %f\n", time, dist, data[i]);
        }
        free(data);
    }

    fprintf(fp,"gmt psxy -R%f/%f/%f/%f -JX8i/6i -K -T>plot.ps\n", b, e, distmin, distmax);
    fprintf(fp,"gmt surface %s -R -I%f/%f -G%s.grd\n", argv[2], delta*10, (distmax-distmin)/100,argv[2]);
    fprintf(fp,"gmt grd2cpt %s.grd -Cseismic>tmp.cpt\n",argv[2]);
    fprintf(fp,"gmt grdimage %s.grd -R -J -K -O -Bx%f+l\"Time(sec)\" -By%f+l\"Distance(degree)\" -BWSen -Ctmp.cpt>>plot.ps\n",argv[2], (e-b)/20, (distmax-distmin)/10);
    fprintf(fp,"gmt psxy -R -J -O -T>>plot.ps\n");
    fprintf(fp,"ps2pdf plot.ps plot.pdf\n");
    fclose(f);
    fclose(fout);
    fclose(fp);
    system("sh plot.sh");

    return 0;
}

