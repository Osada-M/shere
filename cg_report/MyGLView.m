//
//  MyGLView.m
//  
//
//  Created by 長田将 on 2021/05/30.
//

#import "ViewController.h"
#import "MyGLView.h"

@implementation ViewController

- (void)viewDidLoad {
    [super viewDidLoad];

    NSRect viewFrame = [self.view frame];
    MyGLView *glView = [[MyGLView alloc] initWithFrame:viewFrame];
    glView.translatesAutoresizingMaskIntoConstraints = YES;
    glView.autoresizingMask = NSViewWidthSizable | NSViewHeightSizable;
    [self.view addSubview:glView];
}

@end
