/*
 * This file is part of the SDWebImage package.
 * (c) Olivier Poitrey <rs@dailymotion.com>
 *
 * For the full copyright and license information, please view the LICENSE
 * file that was distributed with this source code.
 */

#import <Foundation/Foundation.h>
#import "SDWebImageManager.h"

@class SDWebImagePrefetcher;

/**
 A token represents a list of URLs, can be used to cancel the download.
 */
@interface SDWebImagePrefetchToken : NSObject <SDWebImageOperation>

/**
 * Cancel the current prefetching.
 */
- (void)cancel;

/**
 list of URLs of current prefetching.
 */
@property (nonatomic, copy, readonly, nullable) NSArray<NSURL *> *urls;

@end

/**
 The prefetcher delegate protocol
 */
@protocol SDWebImagePrefetcherDelegate <NSObject>

@optional

/**
 * Called when an image was prefetched. Which means it's called when one URL from any of prefetching finished.
 *
 * @param imagePrefetcher The current image prefetcher
 * @param imageURL        The image url that was prefetched
 * @param finishedCount   The total number of images that were prefetched (successful or not)
 * @param totalCount      The total number of images that were to be prefetched
 */
- (void)imagePrefetcher:(nonnull SDWebImagePrefetcher *)imagePrefetcher didPrefetchURL:(nullable NSURL *)imageURL finishedCount:(NSUInteger)finishedCount totalCount:(NSUInteger)totalCount;

/**
 * Called when all images are prefetched. Which means it's called when all URLs from all of prefetching finished.
 * @param imagePrefetcher The current image prefetcher
 * @param totalCount      The total number of images that were prefetched (whether successful or not)
 * @param skippedCount    The total number of images that were skipped
 */
- (void)imagePrefetcher:(nonnull SDWebImagePrefetcher *)imagePrefetcher didFinishWithTotalCount:(NSUInteger)totalCount skippedCount:(NSUInteger)skippedCount;

@end

typedef void(^SDWebImagePrefetcherProgressBlock)(NSUInteger noOfFinishedUrls, NSUInteger noOfTotalUrls);
typedef void(^SDWebImagePrefetcherCompletionBlock)(NSUInteger noOfFinishedUrls, NSUInteger noOfSkippedUrls);

/**
 * Prefetch some URLs in the cache for future use. Images are downloaded in low priority.
 */
@interface SDWebImagePrefetcher : NSObject

/**
 * The web image manager used by prefetcher to prefetch images.
 * @note You can specify a standalone manager and downloader with custom configuration suitable for image prefetching. Such as `currentDownloadCount` or `downloadTimeout`.
 */
@property (strong, nonatomic, readonly, nonnull) SDWebImageManager *manager;

/**
 * Maximum number of URLs to prefetch at the same time. Defaults to 3.
 */
@property (nonatomic, assign) NSUInteger maxConcurrentPrefetchCount;

/**
 * The options for prefetcher. Defaults to SDWebImageLowPriority.
 * @deprecated Prefetcher is designed to be used shared and should not effect others. So in 5.15.0 we added API  `prefetchURLs:options:context:`. If you want global control, try to use `SDWebImageOptionsProcessor` in manager level.
 */
@property (nonatomic, assign) SDWebImageOptions options API_DEPRECATED("Use individual prefetch options param instead", macos(10.10, API_TO_BE_DEPRECATED), ios(8.0, API_TO_BE_DEPRECATED), tvos(9.0, API_TO_BE_DEPRECATED), watchos(2.0, API_TO_BE_DEPRECATED));

/**
 * The context for prefetcher. Defaults to nil.
 * @deprecated Prefetcher is designed to be used shared and should not effect others. So in 5.15.0 we added API  `prefetchURLs:options:context:`. If you want global control, try to use `SDWebImageOptionsProcessor` in `SDWebImageManager.optionsProcessor`.
 */
@property (nonatomic, copy, nullable) SDWebImageContext *context API_DEPRECATED("Use individual prefetch context param instead", macos(10.10, API_TO_BE_DEPRECATED), ios(8.0, API_TO_BE_DEPRECATED), tvos(9.0, API_TO_BE_DEPRECATED), watchos(2.0, API_TO_BE_DEPRECATED));

/**
 * Queue options for prefetcher when call the progressBlock, completionBlock and delegate methods. Defaults to Main Queue.
 * @deprecated 5.15.0 introduce SDCallbackQueue, use that is preferred and has higher priority. The set/get to this property will translate to that instead.
 * @note The call is asynchronously to avoid blocking target queue. (see SDCallbackPolicyDispatch)
 * @note The delegate queue should be set before any prefetching start and may not be changed during prefetching to avoid thread-safe problem.
 */
@property (assign, nonatomic, nonnull) dispatch_queue_t delegateQueue API_DEPRECATED("Use SDWebImageContextCallbackQueue context param instead, see SDCallbackQueue", macos(10.10, 10.10), ios(8.0, 8.0), tvos(9.0, 9.0), watchos(2.0, 2.0));

/**
 * The delegate for the prefetcher. Defaults to nil.
 */
@property (weak, nonatomic, nullable) id <SDWebImagePrefetcherDelegate> delegate;

/**
 * Returns the global shared image prefetcher instance. It use a standalone manager which is different from shared manager.
 */
@property (nonatomic, class, readonly, nonnull) SDWebImagePrefetcher *sharedImagePrefetcher;

/**
 * Allows you to instantiate a prefetcher with any arbitrary image manager.
 */
- (nonnull instancetype)initWithImageManager:(nonnull SDWebImageManager *)manager NS_DESIGNATED_INITIALIZER;

/**
 * Assign list of URLs to let SDWebImagePrefetcher to queue the prefetching. It based on the image manager so the image may from the cache and network according to the `options` property.
 * Prefetching is separate to each other, which means the progressBlock and completionBlock you provide is bind to the prefetching for the list of urls.
 * Attention that call this will not cancel previous fetched urls. You should keep the token return by this to cancel or cancel all the prefetch.
 *
 * @param urls list of URLs to prefetch
 * @return the token to cancel the current prefetching.
 */
- (nullable SDWebImagePrefetchToken *)prefetchURLs:(nullable NSArray<NSURL *> *)urls;

/**
 * Assign list of URLs to let SDWebImagePrefetcher to queue the prefetching. It based on the image manager so the image may from the cache and network according to the `options` property.
 * Prefetching is separate to each other, which means the progressBlock and completionBlock you provide is bind to the prefetching for the list of urls.
 * Attention that call this will not cancel previous fetched urls. You should keep the token return by this to cancel or cancel all the prefetch.
 *
 * @param urls            list of URLs to prefetch
 * @param progressBlock   block to be called when progress updates; 
 *                        first parameter is the number of completed (successful or not) requests, 
 *                        second parameter is the total number of images originally requested to be prefetched
 * @param completionBlock block to be called when the current prefetching is completed
 *                        first param is the number of completed (successful or not) requests,
 *                        second parameter is the number of skipped requests
 * @return the token to cancel the current prefetching.
 */
- (nullable SDWebImagePrefetchToken *)prefetchURLs:(nullable NSArray<NSURL *> *)urls
                                          progress:(nullable SDWebImagePrefetcherProgressBlock)progressBlock
                                         completed:(nullable SDWebImagePrefetcherCompletionBlock)completionBlock;

/**
 * Assign list of URLs to let SDWebImagePrefetcher to queue the prefetching. It based on the image manager so the image may from the cache and network according to the `options` property.
 * Prefetching is separate to each other, which means the progressBlock and completionBlock you provide is bind to the prefetching for the list of urls.
 * Attention that call this will not cancel previous fetched urls. You should keep the token return by this to cancel or cancel all the prefetch.
 *
 * @param urls            list of URLs to prefetch
 * @param options         The options to use when downloading the image. @see SDWebImageOptions for the possible values.
 * @param context         A context contains different options to perform specify changes or processes, see `SDWebImageContextOption`. This hold the extra objects which `options` enum can not hold.
 * @param progressBlock   block to be called when progress updates;
 *                        first parameter is the number of completed (successful or not) requests,
 *                        second parameter is the total number of images originally requested to be prefetched
 * @param completionBlock block to be called when the current prefetching is completed
 *                        first param is the number of completed (successful or not) requests,
 *                        second parameter is the number of skipped requests
 * @return the token to cancel the current prefetching.
 */
- (nullable SDWebImagePrefetchToken *)prefetchURLs:(nullable NSArray<NSURL *> *)urls
                                           options:(SDWebImageOptions)options
                                           context:(nullable SDWebImageContext *)context
                                          progress:(nullable SDWebImagePrefetcherProgressBlock)progressBlock
                                         completed:(nullable SDWebImagePrefetcherCompletionBlock)completionBlock;

/**
 * Remove and cancel all the prefeching for the prefetcher.
 */
- (void)cancelPrefetching;


@end
