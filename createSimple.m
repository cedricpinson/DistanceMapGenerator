page_screen_output(0);
page_output_immediately(1);

img = imread('./distance.png'); % Read from file
img = double(img);               % Convert to double
if size(img,3) > 1
    img = img(:,:,1);            % Convert to grayscale if RGB
end
img = img - min(min(img));       % Map darkest black to 0
img = img / max(max(img));       % Map brightest white to 1
dist = makedisttex(img, 1024,1024); % No size reduction needed
imwrite(distquant(dist), 'dist.png');
imwrite(img,'ref.png');
