###########################################
## Bivariate functional family generator ##
###########################################

rm ( list = ls() );

### PARAMETERS ###

N = 100;

S = 2;

nTimeSamples = 500;

T0 = 0.;
T1 = 2.;

library(mvtnorm)

meanPhase = c(0.,0.);
rhoPhase = 0.8;
covPhase = matrix( c(1., rhoPhase, rhoPhase, 1.), ncol = 2, byrow = TRUE ); 


meanNoise = c(0.,0.);
covNoise = matrix( c(0.3, 0.2, 0.2, 0.3), ncol = 2, byrow = TRUE );
widthNoise = 0.1;

freq = 1.; 

### GENERATING RANDOM PERTURBATION AND NOISE

phasePerturbation = 0.5*rmvnorm( N, meanPhase, covPhase );
noisePerturbation = 0.5*rmvnorm( nTimeSamples, meanNoise, covNoise );

x11()
par( mfrow = c(2,1) )
D = 2.5;
plot( phasePerturbation, xlim = c(-D,D), ylim = c(-D,D) )
plot( noisePerturbation, xlim = c(-D,D), ylim = c(-D,D) )


### GENERATING DATA

T = c(T0,T1);

times = seq( from = T[1], to = T[2], len = nTimeSamples )

dataI = matrix( NaN, nrow = N, ncol = nTimeSamples, byrow = TRUE )
dataII = matrix( NaN, nrow = N, ncol = nTimeSamples, byrow = TRUE );


for( iN in 1 : N )
{
  dataI[ iN, ] =  widthNoise * t(noisePerturbation[ ,1 ]) +  sin( 2 * pi * times + phasePerturbation[ iN, 1 ] );
  
  dataII[ iN, ] =  widthNoise * t(noisePerturbation[ ,2 ]) + sin( 2 * pi * times + phasePerturbation[ iN, 2 ] );

}

### VISUALIZING DATA
x11()
par( mfrow = c(2,1) )
matplot( t(dataI), type = 'l')
matplot( t(dataII), type = 'l' );

##### FINER
x11()
par(mfrow = c(2,1) )
matplot( t(dataI[ 1:4,]), type = 'l');
matplot( t(dataII[ 1:4, ]), type = 'l');

### SAVING IMAGES
postscript('I_phase_sine.ps');
matplot( t(dataI), type = 'l');
dev.off();
postscript('II_phase_sine.ps');
matplot( t(dataII), type = 'l' );
dev.off();

postscript('perturbationsPlot.ps');
par( mfrow = c(2,1) );
plot( phasePerturbation, xlim = c(-D,D), ylim = c(-D,D) );
plot( noisePerturbation, xlim = c(-D,D), ylim = c(-D,D) );
dev.off();

### SAVING DATA
write.table( dataI, file = "I.txt", sep = " ", eol = "\n", row.names = FALSE, col.names = FALSE );
write.table( dataII, file = "II.txt", sep = " ", eol = "\n", row.names = FALSE, col.names = FALSE);
