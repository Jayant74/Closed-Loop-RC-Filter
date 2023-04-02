%% file:  Setup_HIL
% Dr. Tom Chmielewski
%% System Parameters
% a = 1/(RC)
a = 23.09; % radians  nominal value, adjust to exact measured value
Fs = 400; % Hz   sampling frequency - adjust
T = 1/Fs;

K = 1; % adjust
% try K = 10 you get an overshot with time delay approx
% K sets pole of analog system to
pa = -a-K*a % we want a high pole for fast response

%%
Tsim = 0.0001 % for Simulink fixed point solver: normal: 0.0001

%% z transform model G(z) 
% see notes
zp=exp(-a*T)
zg=K*(1-exp(-a*T))

%% Pole location
pole = (exp(-a*T)*(1+K))-K
