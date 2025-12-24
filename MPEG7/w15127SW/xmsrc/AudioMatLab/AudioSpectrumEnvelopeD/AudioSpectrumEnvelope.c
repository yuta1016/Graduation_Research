/* Untested C code to calculate the AudioSpectrumEnvelope descriptor
  from audio content.  Requires fft() and store_slice() routines. */

/* Note: Submitted by Alain de Cheveigne' 02/23/01 */

#include <math.h>

#LOWFREQ = 64                      /* Hz - lower limit of log part */
#HIFREQ = 16384                    /* Hz - upper limit of log part */
#define FRAME_SIZE  32             /* ms - size of analysis window */
#define FRAME_PERIOD FRAME_SIZE/2  /* ms - descriptor sampling period */

#define PI 3.1416
#define MAX(a,b) ((a>b) ? (a) : (b))
#define MIN(a,b) ((a<b) ? (a) : (b))


/* Stuff to handle arrays of doubles */
typedef struct{
  double *x;
  int n;
} da;

da *da_alloc(int size) {
  da *d;
  assert (d = (da *) malloc(sizeof(da)));
  assert (d->x = (double *) malloc(size * sizeof(double)));
  d->n = size;
  return d;
}

void da_free(da *d) {
  if (d && d->x) free(d->x);
  if (d) free (d);
}

void logscale (da *, da *, double, double);
void power_spectrum(da *, da *, double);



/* This function takes an audio signal and calculates
  AudioSpectrumEnvelope at appropriate intervals. Each slice
  is given to a routine called store_slice() that is expected
  to do something useful with it. */

void AudioSpectrumEnvelope(
  da *signal;               /* audio */
  double sampling_rate;     /* Hz */
  double bands_per_oct;     /* resolution desired */
) {
  da *frame, *power, *logspect;
  int j,k,index, frame_size, fft_size;
  double fract;

  frame_size = (int) (FRAME_SIZE * sampling_rate);
  fft_size = (int) ceil (pow(2, log2(frame_size)));

  /* frame buffer, power spectrum, log spectrum */
  frame = da_alloc(frame_size);
  power = da_alloc(fft_size/2);
  logspect = da_alloc(2 + 8 * bands_per_oct);

  /* Running index insures proper frame spacing if the frame
    period is not an exact multiple of the sampling period.  */
  index = 0;
  fract = 0;

  while (index < signal->n - frame_size) {

    /* copy to frame */
    for (j=0; j<frame->n; j++) {
      frame->x[j] = databuffer->x[j+index];
    }

    /* adjust running index */
    fract += sampling_rate*FRAME_PERIOD;
    index += (int) floor(fract);
    fract = fract - floor(fract);

    /* calculate descriptor & store */
    power_spectrum(power, frame, sampling_rate);
    log_scale(logspect, power, bands_per_oct, sampling_rate);
    store_slice(logspect);
  }

  da_free(frame); da_free(power); da_free(logspect);
}


/* This function takes a frame of signal data, applies a window,
  calls an FFT and derives a power spectrum. */

void power_spectrum(da *power, da* frame, double sampling_rate) {
  static double *window, *buffer;
  static int window_size, fft_size;
  int j;

  /* NOTE: window shape is SQUARE ROOT OF RAISED COSINE, rather than
  raised cosine as specified in the standard.  This gives each
  sample equal weight terms of energy.  Standard should be changed.
  No need to worry about lobe shape at this resolution. */

  if (!window) {
    window_size = frame->n;
    assert (window = (double *) malloc(sizeof(double) * window_size);
    for (j=0; j<frame->n; j++) {
      window[j] = sqrt((2-cos(j*2.0*PI/window_size))/2.0);
    }
  }

  if (!buffer) {
    fft_size = 2 * power->n;
    assert (buffer = (double *) malloc(sizeof(double) * fft_size));
  }

  /* paranoia */
  assert (window_size == frame->n);
  assert (fft_size == 2*power->n);
  assert (fft_size >= window_size);

  /* clear buffer, fill with windowed data */
  for (j=0; j<fft_size; j++) buffer[j] = 0.0;
  for (j=0; j<frame->n; j++) {
    buffer[j] = frame->x[j] * window[j];
  }

  /* fft puts real part in first half, imaginary part in second half */
  fft(buffer, fft_size);

  /* power spectrum */
  for (j=0; j<power->n; j++) {
    double a,b;
    a = buffer[j];
    b = buffer[j+buffersize/2];
    power->x[j] = a*a + b*b;
  }
}


/* This function scales the power spectrum logarithmically */

void logscale (da *logspect, da *power, double bands_per_oct, double
sampling_rate) {
  int j;

  /* check size of logspect, clear it */
  assert (logspect->n >= 2 + 8 * bands_per_oct);
  for (j=1; j<logspect->n; j++) logspect->x[j]=0.0;

  /* is it necessary to divide zero-Hz sample in two?  I forget. */
  power->x[0] /= 2;

  /* share each power spectrum sample among appropriate bands */
  for (j=1; j<power->n; j++) {

    /* each sample is the power of a frequency band */
    lo = (sampling_rate / 2) * (j-0.5) / power->n;
    hi = (sampling_rate / 2) * (j+0.5) / power->n;

    /* convert frequency to "band" */
    lo_band = bands_per_oct * log2(lo / LO_FREQ);
    hi_band = bands_per_oct * log2(hi / LO_FREQ);

    /* band 0 is power below LO_FREQ,
      band 2+8*bands_per_oct is power above HI_FREQ */
    lo_band = MIN(2+8*bands_per_oct, MAX(0, lo_band + 1.0));
    hi_band = MIN(2+8*bands_per_oct, MAX(0, hi_band + 1.0));

    /* split into integer and fractionary parts */
    lo_band_i = floor(lo_band);
    hi_band_i = floor(hi_band);
    lo_band_f = lo_band - lo_band_i;
    hi_band_f = hi_band - hi_band_f;

    /* put power into band(s) */
    if (lo_band_i == hi_band_i) {
      /* fully within one band */
      logspect->x[lo_band_i] += power->x[j];
    } else {
      /* shared among 2 bands or more */
      /* edge bands: */
      logspect->x[lo_band_i] += power->x[j]*(1-lo_band_f)/(hi_band -
lo_band);
      logspect->x[hi_band_i] += power->x[j]*(hi_band_f)/(hi_band - lo_band);
      /* intermediate bands (this may happen for a few low-frequency
         bands if bands_per_oct > 1): */
      for (k=lo_band_i+1; k<hi_band_i; k++) {
        logspect->[k] += power->x[j] * / (hi_band - lo_band);
      }
    }
  }
}
