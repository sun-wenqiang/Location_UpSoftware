function y=add_environment_noise(x, snr, noise)
n_x = length(x);
n_noise = length(noise);
start_index = randi([1,n_noise-n_x+1]);
P_noise_original=mean(noise.^2);
Podwe_signal_target = P_noise_original*10^(snr/10);
A = sqrt(Podwe_signal_target*2);
y = A*x+noise(start_index:start_index+n_x-1);
end