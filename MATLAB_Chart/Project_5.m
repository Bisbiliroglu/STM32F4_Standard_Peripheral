clear all;
clc;
close all;
delete(instrfindall);

comport = serial('COM6','BaudRate',115200, 'DataBits',8);
set(comport,'Parity','none');
fopen(comport);

maxValue = 400;
x=0;

while(x<inf)
axis([0 maxValue -4500 4500]);
x=x+1;
if(x > maxValue)
    f = maxValue;
end
if(x <= maxValue)
    f=x;
end

y1(f)=fscanf(comport, '%d');  %ADC1  
y2(f)=fscanf(comport, '%d');  %ADC2 
%farklý y ler eklenerek daha fazla grafik cizdirilebilir
grid on;
plot(y1,'r','linewidth',1)
hold on;

plot(y2,'b','linewidth',1)
hold on;

title('ADC Testing');
xlabel('Time in mSecond');
ylabel('ADC Values');

drawnow;
hold off;

if(x>=maxValue)
    loop = 0;
    while(loop < (maxValue-1))
        loop = loop +1;
        y1(loop) = y1(loop+1);
    end

    loop = 0;
    while(loop < (maxValue-1))
        loop = loop +1;
        y2(loop) = y2(loop+1);
    end
end

%pause(0.1);
end
fclose(comport);
delete(comport);
clear all;
clc;
close all;