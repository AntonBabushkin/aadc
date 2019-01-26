clear all; close all;

% ADC specs
vref = 2.4;% ADC reference voltage 2.4V
n_bits = 10;% ADC resolution in bits
one_lsb_in_volts = vref/2^n_bits;

% Create test tone
fs = 1;% Sampling frequency 1 - normalized sampling frequency
fsig = 0.005;% Test tone frequency (should be < Nyquist freq)
n_pers = 500;% Amount of periods
t = (0:1/fs:(n_pers/fsig)-1/fs)';% Time vector
vin = 0.49*(2*vref)*sin(2*pi*fsig*t);
L=length(t);
NFFT=2^nextpow2(L);% FFT order
fv=fs/2*linspace(0,fs,NFFT/2+1);% Frequency Vector
window_ = hanning(L);% Use Hanning window


% Run ADC simulation
code = zeros(size(vin));
tic
for i = 1:size(vin,2)
    dlmwrite('vin.csv',vin(:,i),'delimiter','\n','precision',15);
    command = '.\..\Release\aadc.exe tc_3';
    [status,cmdout] = system(command);
    code(:,i) = csvread('code.csv');
end
sim_elapsed_time = toc;
disp(['ADC simulation took ' num2str(sim_elapsed_time/1) ' seconds on average']);

% Compute SNDR (=THD+SNR), and ENOB
fft_vin=fft(vin.*window_,NFFT)/L;
fft_code=fft((code/2^n_bits).*window_,NFFT)/L;
power_tot=(abs(fft_code)*L).^2;% Total power vector
fbin=find(power_tot(1:NFFT/2) == max(power_tot(1:NFFT/2)));% Find peak index
sbin=round(fbin*0.2);% Test tone spectrum
nbin1=fbin-sbin; nbinr=fbin+sbin;
signalbin=nbin1:nbinr;
signal=sum(power_tot(signalbin));% Test tone power
noisebin=[1:nbin1-1 nbinr+1:NFFT/2+1];
noise=sum(power_tot(noisebin));% Noise power
sndr=10*log10(signal/noise);% Compute SNDR
enob=(sndr-1.76)/6.02;% Compute ENOB


% % Compute SNDR using MATLAB's sinad() function
% sndr_sinad = sinad((code/2^n_bits));
% figure();
% sinad((code/2^n_bits));


% % Plot test tone and output in time domain
% figure();
% yyaxis left
% plot(t, vin)
% yyaxis right
% plot(t, code)
% title('vin and output')
% legend('vin', 'code')


% % Plot spectrum of test tone
% figure()
% semilogx(fv, 20*log(2*abs(fft_vin(1:NFFT/2+1))));
% title('Spectrum of test tone');
% xlabel('Normalized frequency');
% ylabel('Amplitude [dB]');
% xlim([0 fs/2])


% Plot spectrum of output code
ai = 1;
figure()
semilogx(fv,20*log10(2*abs(fft_code(1:NFFT/2+1))));
ylim([-300 0]);
title('Frequency spectrum of output code');
ylabel('Amplitude [dB]');
ylabel('Normalized frequency');
xlim([0 fs/2])
text(fv(fbin), -20, [' SNDR: ' num2str(sndr)]);
text(fv(fbin), -40, [' ENOB: ' num2str(enob)]);


% Display statistics
disp(['SNDR: ' num2str(sndr)])
disp(['ENOB: ' num2str(enob)])

