//
//  AppDelegate.h
//  MacClient2
//
//  Created by Benoît et Kathy on 2013-05-08.
//
//

#import <Cocoa/Cocoa.h>
#import <MacFreeRDP-library/MRDPView.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

@property (assign) IBOutlet NSWindow *window;
@property (assign) IBOutlet MRDPView *mrdpView;

@end
