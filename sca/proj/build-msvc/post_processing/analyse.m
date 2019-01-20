clear all; close all;

load 'vin_code_dnl.mat'

vin = vin_code_dnl(:,1,:);
code = vin_code_dnl(:,2,:);
dnl = vin_code_dnl(:,3,:);

dnl_err = dnl-1; dnl_err(1:3) = 0;
inl_err = cumsum(dnl_err)+1;

code_ideal = polyval(polyfit(vin,code,2),vin);

figure()
plot(code, vin, code_ideal, vin)
title('output code vs Vin')

figure()
plot(code, code_ideal-code)
title('output code non-linearity vs output code')
xlim([-2^10 2^10])

figure()
plot(code, dnl_err)
title('DNL vs output code')
xlim([-2^10 2^10])

figure()
plot(code, inl_err)
title('INL vs output code')
xlim([-2^10 2^10])


