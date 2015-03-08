%% Script to convert a WAV file to a format we can use on the µCU
%
%   [data] = adaptWAVtoArray(filename)
% 
%   This function takes a filename, resamples the audio to the correct
%   samplerate (if necessary), scales (normalizes) the signal to
%   appropriorate levels and returns the output as an integer vector scaled
%   to 12 bits.
function output = adaptWAVtoArray(filename)
    target_fs = 44100;
    target_amp = 0.95;
    [data,fs] = audioread(filename);
    if(fs ~= target_fs)
        % If the samplerate isn't correct, resample
        data = resample(data,target_fs,fs);
    end %if
    % Find signal amplitude
    ampl = max(abs(data));
    if(ampl < target_amp || ampl >= 1)
        % Amplify signal if necessary
        data = data*(target_amp/ampl);
    end %if
    % Move signal from [-1,1] to [0,2]
    data = data +1;
    % Quanize the signal to 12 bit integer values
    output = round(data*pow2(11));
    fprintf('Conversion successful, output values range from %d to %d\n', min(output), max(output));
end %function